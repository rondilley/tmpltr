# Templater (tmpltr)

by Ron Dilley <ron.dilley@uberadmin.com>

You can find the latest information on tmpltr [here](http://www.uberadmin.com/Projects/tmpltr/ "Log Templater")

## What is Templater (tmpltr)?

Templater is a small and fast log processor that provides
simple artificial ignorance capabilities.  You use the tool
to process past log data and store templates that represent
normal log line structures.  You then run the tool against
current or target logs and all normal patterns are automatically
ignored.

The parser is fast and capable of processing millions of
lines per minute.  For weblogs and firewall logs, I average
9M lines per minute on a 2GHz x86 machine running *NIX.

The template strategy was originally proposed by a friend
of mine in 2003 who later built a tool called never before
seen (NBS*) which also provides artificial ignorance for
arbitrary text data as well as text structures.

[Marcus Ranum's NBS](http://www.ranum.com/security/computer_security/code/nbs.tar "Never Before Seen")

## Why use it?

I built this tool to solve a log analysis problem that I have
suffered through while responding to many security breaches.
Invariably, I need to find a needle in a haystack of log data.

If you need to find a pattern that has not occurred previously,
then this is the tool for you.

## Implementation

Templater has a simple command lines interface.  In it's
simplest form, pass a text file as an argument and the output
will be list of counts and unique templates with the first
full log line separated by a '||'.

Including the first full log line with each unique template
allows for quick recognition of the log lines that map to
the unique template.

To get a list of all the options, you can execute the
command with the -h or --help switch.

```
tmpltr v0.10 [Jan 14 2021 - 16:56:30]

syntax: tmpltr [options] filename [filename ...]
 -c|--cluster           show invariable fields in output
 -d|--debug (0-9)       enable debugging info
 -g|--greedy            ignore quotes
 -h|--help              this info
 -l|--line {line}       show all lines that match template of {line}
 -L|--linefile {fname}  show all the lines that match templates of lines in {fname}
 -m|--match {template}  show all lines that match {template}
 -M|--matchfile {fname} show all the lines that match templates in {fname}
 -n|--cnum {num}        max cluster args [default: 1]
 -t|--templates {file}  load templates to ignore
 -v|--version           display version information
 -w|--write {file}      save templates to file
 filename               one or more files to process, use '-' to read from stdin
```

The debug option is most useful when the tool is compiled
with the --ENABLE-DEBUG switch.

A typical run of tmpltr is to pass the target log file as an
argument and send the output through 'sort -n' to produce
the following sorted list of unique templates and their
frequency of occurent from least to most prevalent.

I normally go through the list in this order to speed up
catching the anonlylous patterns.

```
% tmpltr /var/log/secure.log | sort -n
Opening [/var/log/secure.log] for read
           1 %x %d %s %s %s[%d]:     %s : %s=%s ; %s=/%s/%s/%s/%s/%s ; %s=%s ; %s=./%s/%s/%s/%s/%s /||Dec 17 13:05:49 Rons-MacBook-Pro sudo[7974]:     root : TTY=unknown ; PWD=/private/tmp/PKInstallSandbox.WSZSYc/Scripts/com.apple.pkg.Safari6.1.1Lion.wqQJEF ; USER=rdilley ; COMMAND=./Tools/AlertAll.app/Contents/MacOS/AlertAll /
           1 %x %d %s %s %s[%d]: %s %s %s %s %s %s>%s||Dec 16 16:30:01 Rons-MacBook-Pro newsyslog[5373]: logfile turned over due to size>1000K
           1 %x %d %s %s %s[%d]: %s %s %s %s /%s/%s||Dec 16 20:01:32 Rons-MacBook-Pro su[5799]: rdilley to root on /dev/ttys001
           1 %x %d %s %s %s[%d]: %s(?:%s) %s %s=-%d||Dec 17 02:02:36 Rons-MacBook-Pro com.apple.SecurityServer[24]: setup(?:obsolete) failed rcode=-2147418111
           2 %x %d %s %s %s[%d]: %s %s %s %s '%s' %s %s '%s' [%d] %s %s %s %s '%s' [%d]||Dec 28 15:10:13 Rons-MacBook-Pro com.apple.SecurityServer[24]: Failed to authorize right 'system.install.app-store-software' by client '/System/Library/PrivateFrameworks/PackageKit.framework/Versions/A/Resources/installd' [12669] for authorization created by '/System/Library/CoreServices/Software Update.app' [12661]
           5 %x %d %s %s %s[%d]: %s %s %s||Dec 17 15:25:21 Rons-MacBook-Pro com.apple.SecurityServer[24]: Killing auth hosts
           8 %x %d %s %s %s[%d]: %s %s %s %s %s %s %x %s||Dec 17 17:18:04 Rons-MacBook-Pro loginwindow[58]: resume called when there was already a timer
          10 %x %d %s %s %s[%d]: %s %d %s||Dec 17 15:02:23 Rons-MacBook-Pro com.apple.SecurityServer[24]: Session 100012 created
          50 %x %d %s %s %s[%d]: %s %s(): %s %d %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Kerberos 5 refuses you
          50 %x %d %s %s %s[%d]: %s %s(): %s %s()||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Done getpwnam()
          50 %x %d %s %s %s[%d]: %s %s(): %s %s, %s: %d %d||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Got euid, egid: 502 20
          50 %x %d %s %s %s[%d]: %s %s(): %s: %s %s %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): pam_sm_setcred: krb5 user rdilley doesn't have a principal
          51 %x %d %s %s %s[%d]: %s %s(): %s %s %s %s %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Failed to determine Kerberos principal name.
          51 %x %d %s %s %s[%d]: %s %s(): %s %s %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in od_principal_for_user(): No authentication authority returned
          53 %x %d %s %s %s[%d]: %s %s(): %s %d %s %s %s:%s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in od_record_attribute_create_cfstring(): returned 2 attributes for dsAttrTypeStandard:AuthenticationAuthority
          53 %x %d %s %s %s[%d]: %s %s(): %s - %s %s %s %s %s %d.||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_acct_mgmt(): OpenDirectory - Membership cache TTL set to 1800.
         100 %x %d %s %s %s[%d]: %s %s(): %s %s() & %s()||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Done setegid() & seteuid()
         103 %x %d %s %s %s[%d]: %s %s(): %s: %d||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in od_principal_for_user(): failed: 7
         200 %x %d %s %s %s[%d]: %s %s(): %s %s: %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Got user: rdilley
         252 %x %d %s %s %s[%d]: %s %s(): %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Done cleanup3
        1197 %x %d %s --- %s %s %s %d %s ---||Dec 16 16:31:44: --- last message repeated 1 time ---
        3119 %x %d %s %s %s[%d]: %s %s %s '%s' %s %s '%s' [%d] %s %s %s %s '%s' [%d]||Dec 16 16:30:44 Rons-MacBook-Pro com.apple.SecurityServer[24]: Succeeded authorizing right 'system.preferences' by client '/System/Library/PrivateFrameworks/Admin.framework/Versions/A/Resources/writeconfig' [3274] for authorization created by '/Applications/System Preferences.app' [3268]
```

If the standard templating mode is obscuring too much information, you can switch to clustering mode (-c).  This reduces the parsing speed, but allows the templates to retain all of the non-variable strings.  I normally strip off the trailing example line when running in this mode just to keep the line length more managable.  Below is an example of the same log processed with the clustering (-c) parsing option.

```% tmpltr -c /var/log/secure.log | sort -n
Opening [/var/log/secure.log] for read
           1 Dec 16 16:30:01 Rons-MacBook-Pro newsyslog[5373]: logfile turned over due to size>1000K
           1 Dec 16 20:01:32 Rons-MacBook-Pro su[5799]: rdilley to root on /dev/ttys001
           1 Dec 17 02:02:36 Rons-MacBook-Pro com.apple.SecurityServer[24]: setup(?:obsolete) failed rcode=-2147418111
           1 Dec 17 13:05:49 Rons-MacBook-Pro sudo[7974]:     root : TTY=unknown ; PWD=/private/tmp/PKInstallSandbox.WSZSYc/Scripts/com.apple.pkg.Safari6.1.1Lion.wqQJEF ; USER=rdilley ; COMMAND=./Tools/AlertAll.app/Contents/MacOS/AlertAll /
           2 Dec 28 15:10:13 Rons-MacBook-Pro com.apple.SecurityServer[24]: Failed to authorize right '%s' by client '/System/Library/PrivateFrameworks/PackageKit.framework/Versions/A/Resources/installd' [12669] for authorization created by '/System/Library/CoreServices/Software Update.app' [12661]||Dec 28 15:10:13 Rons-MacBook-Pro com.apple.SecurityServer[24]: Failed to authorize right 'system.install.app-store-software' by client '/System/Library/PrivateFrameworks/PackageKit.framework/Versions/A/Resources/installd' [12669] for authorization created by '/System/Library/CoreServices/Software Update.app' [12661]
           5 Dec %d %s Rons-MacBook-Pro com.apple.SecurityServer[24]: Killing auth hosts||Dec 17 15:25:21 Rons-MacBook-Pro com.apple.SecurityServer[24]: Killing auth hosts
           8 Dec %d %s Rons-MacBook-Pro loginwindow[58]: resume called when there was already a timer||Dec 17 17:18:04 Rons-MacBook-Pro loginwindow[58]: resume called when there was already a timer
          10 Dec %d %s Rons-MacBook-Pro com.apple.SecurityServer[24]: Session %d %s||Dec 17 15:02:23 Rons-MacBook-Pro com.apple.SecurityServer[24]: Session 100012 created
          50 Dec %d %s Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Kerberos 5 refuses you||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Kerberos 5 refuses you
          50 Dec %d %s Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Done getpwnam()||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Done getpwnam()
          50 Dec %d %s Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Got euid, egid: 502 20||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Got euid, egid: 502 20
          50 Dec %d %s Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): pam_sm_setcred: krb5 user rdilley doesn't have a principal||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): pam_sm_setcred: krb5 user rdilley doesn't have a principal
          51 Dec %d %s Rons-MacBook-Pro %s[%d]: in %s(): %s %s %s %s %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Failed to determine Kerberos principal name.
          51 Dec %d %s Rons-MacBook-Pro %s[%d]: in %s(): %s %s %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in od_principal_for_user(): No authentication authority returned
          53 Dec %d %s Rons-MacBook-Pro %s[%d]: in od_record_attribute_create_cfstring(): returned 2 attributes for dsAttrTypeStandard:AuthenticationAuthority||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in od_record_attribute_create_cfstring(): returned 2 attributes for dsAttrTypeStandard:AuthenticationAuthority
          53 Dec %d %s Rons-MacBook-Pro %s[%d]: in pam_sm_acct_mgmt(): OpenDirectory - Membership cache TTL set to 1800.||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_acct_mgmt(): OpenDirectory - Membership cache TTL set to 1800.
         100 Dec %d %s Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Done %s() & %s()||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_setcred(): Done setegid() & seteuid()
         103 Dec %d %s Rons-MacBook-Pro %s[%d]: in %s(): %s: %d||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in od_principal_for_user(): failed: 7
         200 Dec %d %s Rons-MacBook-Pro loginwindow[58]: in %s(): Got %s: %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Got user: rdilley
         252 Dec %d %s Rons-MacBook-Pro %s[%d]: in %s(): %s %s||Dec 16 16:35:18 Rons-MacBook-Pro loginwindow[58]: in pam_sm_authenticate(): Done cleanup3
        1197 Dec %d %s --- last message repeated %d %s ---||Dec 16 16:31:44: --- last message repeated 1 time ---
        3119 Dec %d %s Rons-MacBook-Pro com.apple.SecurityServer[24]: Succeeded authorizing right '%s' by client '%s' [%d] for authorization created by '%s' [%d]||Dec 16 16:30:44 Rons-MacBook-Pro com.apple.SecurityServer[24]: Succeeded authorizing right 'system.preferences' by client '/System/Library/PrivateFrameworks/Admin.framework/Versions/A/Resources/writeconfig' [3274] for authorization created by '/Applications/System Preferences.app' [3268]
```

You can use the same syntax but also store copies of all
of the unique templates by using the '-w {fname}' switch.
When you do this, the file references with the switch is
over written with a <CR> delimited list of the templates.

The currently supported field types are as follows:

| placeholder | meaning |
| -----------:| ------- |
|%c | character |
|%d | number |
|%f | float |
|%x | hex number |
|%s | string |
|%i | IPv4 address (number-dot syntax) |
|%I | IPv6 |
|%m | MAC address (IEEE 802 syntax) |
|%t | time (2020-12-17 00:14:59[.000]) |

The template file can be used with the '-t {fname}' switch
to provide artificial ignorance capabilities.

```
% tmpltr -w ignore.templates /var/log/kernel.log | sort -n
```

After the above run of the tool, a file named ignore.templates
is created in the current directory as shown below:

```
$ cat ignore.templates

%x %d %s %s %s[%d]: %s(%s) == %d
%x %d %s %s %s[%d]: %s %d, %s %d, %s %d, %s %d, %s %d, %s %d, %s %d, %s %s %s %d %s %d %s %d %s %d
%x %d %s %s %s[%d]: %s
%x %d %s %s %s[%d]: %s %s %s %s %s %s %s
%x %d %s %s %s[%d]: %s [%s]: %s %s %s %s (%s %s, %s %d)
%x %d %s %s %s[%d]: %s: %s %s %s %s %s %d (%s %s %s %s).
%x %d %s %s %s[%d]: %s: %s %s %d %s %s %s %s(%c)
%x %d %s %s %s[%d]: %s: %d.%x %s %s %s %s '%s'.
%x %d %s %s %s[%d]: %s %x
%x %d %s %s %s[%d]: %s: %s %s %s %s %s = %d
%x %d %s %s %s[%d]: %s::%s - %s, %s %s, %x %s, %s %s %s
%x %d %s %s %s[%d]: [%s](%d)/(%d) %s %s %s
%x %d %s %s %s[%d]: %s %s %s: /%s/%s/%s
%x %d %s %s %s[%d]: [%s %s] 
%x %d %s %s %s[%d]: %s: %s %s %s %s %s
%x %d %s %s %s[%d]: %s %s %s %s
%x %d %s %s %s[%d]: %s, %s %d
%x %d %s %s %s[%d]: %s %s %s %s %s %s>%s
%x %d %s %s %s[%d]: %s %d, %s %d, %s %d
%x %d %s %s %s[%d]: %s: %s %s %x:%s
%x %d %s %s %s[%d]: 
%x %d %s %s %s[%d]: %s: %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d
%x %d %s %s %s[%d]: %s %s %s %s %s (%d)
%x %d %s %s %s[%d]: %s: %s %s %s %m
%x %d %s %s %s[%d]: %s /%s/%s %s/.%s/%s/%x-%x-%x-%x-%x/%s] [%s %d] [%s %s] 
%x %d %s %s %s[%d]: %s %s   %s %s %s: %m %s %s %s
%x %d %s %s %s[%d]: %s: %s: %s: %s %s %s %s %s!
%x %d %s %s %s[%d]: %s: %s %d %s %s %s (%x         %d [%s %d] [%s %d] [%s %s] [ [%s %d] [%s %s] 
%x %d %s %s %s[%d]: %s: %s %s %s %s, %s #%d, %s %m
%x %d %s %s %s[%d]: %s %x %s %d
%x %d %s %s %s[%d]: %s %s(%d): %d []
%x %d %s %s %s[%d]: %s: %s %s
%x %d %s %s %s[%d]: [ %s %s %s, %s %d %s %d ]
%x %d %s %s %s[%d]: %s: %s %d %s %s %s (%s %s)
%x %d %s %s %s[%d]: %s %s %s (%s=%d %s=%d %s=%d)
%x %d %s %s %s[%d]: %s<%s>::%s - %s - %s [%s=%d %s=%d %s=%d] %s %s [%s=%d %s=%d %s=%d]
%x %d %s %s %s[%d]: %s: %s %s=%d
%x %d %s %s %s[%d]: %s %d, %s %d (%d), %s %d (%d%c), %s %x, %s %x
%x %d %s %s %s[%d]: %s %s %s %d
%x %d %s %s %s[%d]: %s %s %s, %s
%x %d %s %s %s[%d]: %s %s, %s %s, %s %s
%x %d %s %s %s[%d]: %s: %s %s %s
%x %d %s %s %s[%d]: %s %d
%x %d %s %s %s[%d]: %s %s %s %d, %s %d, %s %d, %s %d
%x %d %s %s %s[%d]: %s: %s %s %s %s
%x %d %s %s %s[%d]: %s %s] [%s /%s/%s] [%s /%s/%s] [%s %d] [%s %s] 
%x %d %s %s %s[%d]: %s %s(%d): %d [%c]
%x %d %s %s %s[%d]: %s: %s %s %s %d %s
%x %d %s %s %s[%d]: [ %s %s %s ]
%x %d %s %s %s[%d]: %s: %s: %s: %s %s %s
%x %d %s %s %s[%d]: %s %s %s %s (%s %d %s %s %s %s) %s %s %s %x %s %s %s %x %s %s (%d)
%x %d %s %s %s[%d]: %s %s %s %s %s %s / %s (%s %d %s %s %s %s) %s %s %s %x %s %s %s %x %s %s (%d)
%x %d %s %s %s[%d]: %s::%s - %s
%x %d %s %s %s[%d]: %s %s %d, %s %d %s
%x %d %s %s %s[%d]: %s: %s %d %s %s %s (%s)
%x %d %s %s %s[%d]: %s: %s %d %s / %s %s %s %d %s 
%x %d %s %s %s[%d]: %s %s: %s(%s): %c=%d[%s] %s %s
%x %d %s %s %s[%d]: %s %s: ?
%x %d %s %s %s[%d]: %s<%s>::%s - %s %s
%x %d %s %s %s[%d]: %s %s   %s %s %s: %m %s %s
%x %d %s --- %s %s %s %d %s ---
%x %d %s %s %s[%d]: %s %d %s
%x %d %s %s %s[%d]: %s (%s) %s %s %d %s %s %s, %s %x; %s %s %s
%x %d %s %s %s[%d]: %s %s: %x.%s (%s)
%x %d %s %s %s[%d]: %s %s   %s %s %s: %m  %s %s %s
%x %d %s %s %s[%d]: %s: %s: %s: %s: %d %s: %d (%s %s)
%x %d %s %s %s[%d]: %s: %s: %s: %s %s, %s %s  %s %s %s
%x %d %s %s %s[%d]: %s [%s]: %s %s %s %s, %d-%s, %s, %s %s, %s [%x,%d,%x,%d,%x,%d]
%x %d %s %s %s[%d]: %s::%s - %s %s %s, %s %d: %s %s %s
%x %d %s %s %s[%d]: %s(%d)
%x %d %s %s %s[%d]: %s %s %s: %d
%x %d %s %s %s[%d]: %s(%d) %s %d %s
%x %d %s %s %s[%d]: %s %s /%s/%s/%s, %s %d, %s %s %s, %s %d %s %d
%x %d %s %s %s[%d]: %s [%s]: %s %s %s %s, %d-%s, %s, %s %s, %s [%x,%d,%d,%d,%x,%d]
%x %d %s %s %s[%d]: %s %s: %d %s
%x %d %s %s %s[%d]: %s: %s/%s %s %s %s
%x %d %s %s %s[%d]: %s(%s)::%s - %s %s %s %s %s = %s %s = %d %s = %d
%x %d %s %s %s[%d]: %s %d, %s %d (%d), %s %d (%d%c), %s %x, %s %d
%x %d %s %s %s[%d]: %s %s(%d)
%x %d %s %s %s[%d]: %s [%s]: %s %s %s %s (%s)
%x %d %s %s %s[%d]: %s %s %d %s
%x %d %s %s %s[%d]: %s %s: %d %s, %s %s: %d %s: %d %s %d %s/%c, %s %s: %d %s: %d %s %d %s/%c, 
%x %d %s %s %s[%d]: %s %s %s %s %s (%s %d %s %s %s %s) %s %s %s %x %s %s %s %x %s %s (%d)
%x %d %s %s %s[%d]: %s::_%s - %s %s %s %s %s %s %s %s - %s
%x %d %s %s %s[%d]: %s [%s]: %s %s %s %s
%x %d %s %s %s[%d]: %d [%s %d] [%s %d] [%s %s] [%s /%s/%s] [%s /%s/%s %s] [%s /%s/%s %s/.%s/%s/%x-%x-%x-%x-%x/%s] [%s %d] [%s %s] 
%x %d %s %s %s[%d]: %s::%s %s %s=%s (%s %s %s) %s=%s
%x %d %s %s %s[%d]: %s (%s: %s) %s %s %s %s %s %s %s: %s, %s %s>%s %s %s %s %s %s, %s %s %s %s %s %s %s %s %s
%x %d %s %s %s[%d]: %s %s (%s): %x %s %s %s
%x %d %s %s %s[%d]: %s: %s: %s %s %s
%x %d %s %s %s[%d]: %s %s %s %s %s (%x) %d %s %s
%x %d %s %s %s[%d]: %s %s: %x.%s %s (%s)
```

You can now pass this file back into tmpltr and all
log lines that match an existing template pattern will
be ignored.  This is the real utility of the tool.  Using
this technique, you can train tmpltr to ignore all log
patterns that have been seen before and only show log
templates for new patterns.

This is how I use to tool during incident response.  I
push all the log data that I have for the period of my
choice before the detected event into tmpltr and save
the pattern file, then run tmpltr against the log data for
the period around when the detected event happened.

```
$ ./tmpltr -t ignore.templates /var/log/system.log
```

A couple of things to note about the '-w' and '-t' switches:

If you use the '-w' switch in conjunction with the '-t',
you should not read from and write to the same file.  This
version of tmpltr would allow you to do it without an issue
if you pass the read '-t {fname}' arguments before the
write '-w {fname}' arguments but future versions may not
work as expected.

Additionally, when you load templates using the '-t' switch
then write all the templates using the '-w' switch, then
new file will contain all of the templates loaded from the
template file as well as any new unique templates discovered
in the processing of the log files.

In this way, you can have your most recent saved template file
always contain all of the templates that have been discovered.

By default, the tool assumes that anything inside of a pair
of double quotes is a single string.  This is really helpful
when processing log lines where unstructured data is included
bounded by double quotes.  The user agent strings in Apache
web logs is a good example of this.  There are times though
when it is really nice to be able to tokenize the data between
those double quotes.  You can use the greedy (-g) switch to
ignore the double quotes and everything between them will be
parsed the same was as all other patterns in the line.

## Security Implications

Assume that there are errors in the tmpltr source that
would allow a specially crafted logs to allow an attacker
to exploit tmpltr to gain access to the computer that it is
running on!!!  Don't trust this software and install and use
it at your own risk.

## Bugs

I am not a programmer by any stretch of the imagination.  I
have attempted to remove the obvious bugs and other
programmer related errors but please keep in mind the first
sentence.  If you find an issue with code, please send me
an e-mail with details and I will be happy to look into
it.

Ron Dilley
ron.dilley@uberadmin.com
