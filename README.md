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
lines per minute.  For web and firewall logs, it averages
**27M** lines per minute on a 3GHz x86 machine running *NIX.

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
tmpltr v0.14 [Feb 28 2022 - 17:09:47]

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

```% tmpltr /var/log/syslog | sort -n
Opening [/var/log/syslog] for read
           1 %s %d %s %s [%d]: %c_%s: %s '%s > %d' %s||Apr 29 12:50:34 ubuntu [3008013]: g_object_ref: assertion 'old_val > 0' failed
           1 %s %d %s %s %s[%d]: %s `%s' %s||Apr 29 07:35:35 ubuntu anacron[2996986]: Job `cron.daily' started
           1 %s %d %s %s %s[%d]: (%s) %s (%s)||Apr 29 08:00:02 ubuntu crontab[3000587]: (root) LIST (nobody)
           1 %s %d %s %s %s[%d]: %s %s %s %s: %d %s %s %s||Apr 29 11:50:41 ubuntu fwupdmgr[3006998]: Successfully downloaded new metadata: 0 local devices supported
           1 %s %d %s %s %s[%d]: %s %s %s `%s' %s %d %s||Apr 29 07:30:35 ubuntu anacron[2996986]: Will run job `cron.daily' in 5 min.
           1 %s %d %s %s %s[%d]: %s `%s' %s (%s %s: %d) (%s %s)||Apr 29 07:35:36 ubuntu anacron[2996986]: Job `cron.daily' terminated (exit status: 1) (mailing output)
           1 %s %d %s %s %s[%d]: %s %s %s %s `%s' %s %s||Apr 29 07:35:35 ubuntu anacron[2997066]: Updated timestamp for job `cron.daily' to 2021-04-29
           1 %s %d %s %s %s[%d]: (%s) %s (%s -%s/%s/%s/%s || ( %x / && %s --%s /%s/%s ))||Apr 29 06:25:01 ubuntu CRON[2994686]: (root) CMD (test -x /usr/sbin/anacron || ( cd / && run-parts --report /etc/cron.daily ))
           1 %s %d %s %s %s[%d]: (%s) %s (%s -%x /%s/%s/%s || %s=%d /%s/%s -%x -%c)||Apr 29 03:10:01 ubuntu CRON[2987654]: (root) CMD (test -e /run/systemd/system || SERVICE_MODE=1 /sbin/e2scrub_all -A -r)
           1 %s %d %s %s %s[%d]: %s %s %x %s %s||Apr 29 07:30:35 ubuntu anacron[2996986]: Jobs will be executed sequentially
           1 %s %d %s %s %s[%d]: %x %s %s %s %s %s'%s %s||Apr 29 08:08:44 ubuntu gvfsd[2014500]: A connection to the bus can't be made
           1 %s %d %s %s %s: %s %s %s %s||Apr 29 07:35:36 ubuntu cracklib: no dictionary update necessary.
           2 %s %d %s %s %s[%d]: %s %s||Apr 29 08:08:44 ubuntu indicator-appli[2014597]: Name Lost
           2 %s %d %s %s %s[%d]: %s %s %s %s %d %s %s %s %s %f||Apr 29 14:18:46 ubuntu systemd[2238]: Started VTE child process 3011238 launched by gnome-terminal-server process 2695.
           2 %s %d %s %s %s[%d]: %s %s; %s/%s %s %s||Apr 29 08:08:44 ubuntu indicator-datet[2014612]: indicator-datetime exiting; failed/lost bus ownership
           2 %s %d %s %s %s[%d]: %s: %s %s'%s %s %s %s %s %s||Apr 29 08:08:44 ubuntu indicator-power[2014598]: exiting: service couldn't acquire or lost ownership of busname
           2 %s %d %s %s %s[%d]: %s: %s %s'%s, %s %s %s %s, %s||Apr 29 08:08:44 ubuntu indicator-sessi[2014618]: exiting: service couldn't acquire, or lost ownership of, busname
           3 %s %d %s %s %s[%d]: %s:%d: %s: %s %s %s %s||Apr 29 01:33:32 ubuntu snapd[791]: autorefresh.go:479: auto-refresh: all snaps are up-to-date
           3 %s %d %s %s %s[%d]: %s %s %s: ||Apr 29 08:42:57 ubuntu xfce4-notifyd[3002212]: cannot open display: 
           3 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s: %s||Apr 29 08:08:44 ubuntu indicator-sound[2014629]: Name lost or unable to acquire bus: com.canonical.indicator.sound
           3 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s, %s||Apr 29 09:31:32 ubuntu tumblerd[3003112]: Name org.freedesktop.thumbnails.Cache1 lost on the message dbus, exiting.
           3 %s %d %s %s %s/%s[%d]: %x: %s||Apr 29 01:08:00 ubuntu postfix/qmgr[2145]: 119904600DA: removed
           3 %s %d %s %s %s/%s[%d]: %x: %s=%d %s=<%s>||Apr 29 01:08:00 ubuntu postfix/pickup[2015577]: 119904600DA: uid=0 from=<root>
           3 %s %d %s %s %s/%s[%d]: %x: %s=<%s>||Apr 29 01:08:00 ubuntu postfix/cleanup[2984439]: 119904600DA: message-id=<20210429080800.119904600DA@ubuntu.localdomain>
           3 %s %d %s %s %s/%s[%d]: %x: %s=<%s>, %s=%d, %s=%d (%s %s)||Apr 29 01:08:00 ubuntu postfix/qmgr[2145]: 119904600DA: from=<root@ubuntu-dev.uberadmin.com>, size=19652, nrcpt=1 (queue active)
           4 %s %d %s %s %s[%d]: %s: %s %s %s '%s'.||Apr 29 03:00:19 ubuntu systemd[1]: fwupd-refresh.service: Failed with result 'exit-code'.
           4 %s %d %s %s %s[%d]: %s %s %s://%s/%s/%s||Apr 29 03:00:19 ubuntu fwupdmgr[2987489]: Fetching metadata https://cdn.fwupd.org/downloads/firmware.xml.gz
           4 %s %d %s %s %s[%d]: %s: %s %s %s, %s=%s, %s=%d/%s||Apr 29 03:00:19 ubuntu systemd[1]: fwupd-refresh.service: Main process exited, code=exited, status=1/FAILURE
           4 %s %d %s %s %s: %s %s||Apr 29 06:00:25 ubuntu PackageKit: daemon start
           5 %s %d %s %s %s[%d]: %s: %s %s %d (%s) %s %s %s||Apr 29 08:08:44 ubuntu systemd[2014467]: dbus.service: Killing process 2014538 (gdbus) with signal SIGKILL.
           7 %s %d %s %s %s[%d]: %s %f %s %s %s||Apr 29 07:30:35 ubuntu anacron[2996986]: Anacron 2.3 started on 2021-04-29
           7 %s %d %s %s %s[%d]: %s %s (%d %s %s)||Apr 29 07:35:36 ubuntu anacron[2996986]: Normal exit (1 job run)
           7 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s %s %s, %s %s %s %s %s %s %s||Apr 29 08:26:57 ubuntu pulseaudio[3001795]: ALSA woke us up to write new data to the device, but there was actually nothing to write.
           8 %s %d %s %s %s[%d]: %s %s %d %s: [ %s %s %s %s() %s: %s %s %s %s]||Apr 29 08:08:44 ubuntu indicator-sound[2014629]: message repeated 3 times: [ Error while sending AddMatch() message: The connection is closed]
           9 %s %d %s %s %s[%d]: %c_%s: %s '%s > %d' %s||Apr 29 08:08:44 ubuntu indicator-sound[2014629]: g_object_ref: assertion 'old_val > 0' failed
           9 %s %d %s %s %s[%d]: %s[%d]: %s %s %s '%s'||Apr 29 08:26:58 ubuntu at-spi-bus-launcher[3001881]: dbus-daemon[3001881]: Successfully activated service 'org.a11y.atspi.Registry'
           9 %s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s %s %s||Apr 29 08:26:57 ubuntu dbus-daemon[3001807]: [session uid=128 pid=3001807] AppArmor D-Bus mediation is enabled
           9 %s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s '%x.%s'||Apr 29 08:26:58 ubuntu dbus-daemon[3001807]: [session uid=128 pid=3001807] Successfully activated service 'ca.desrt.dconf'
           9 %s %d %s %s %s[%d]: %s: %s '%s (%s)' %s||Apr 29 08:26:58 ubuntu indicator-keybo[3001919]: gtk_icon_theme_get_for_screen: assertion 'GDK_IS_SCREEN (screen)' failed
           9 %s %d %s %s %s[%d]: %s %s %s %s %s (%s %s).||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on GnuPG cryptographic agent (ssh-agent emulation).
           9 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s - %s||Apr 29 08:26:58 ubuntu at-spi-bus-launcher[3001909]: SpiRegistry daemon is running with well-known name - org.a11y.atspi.Registry
           9 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s (%s).||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on GnuPG cryptographic agent and passphrase cache (restricted).
           9 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s %s||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on REST API socket for snapd user session agent.
           9 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s (%s %s %s %s).||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on GnuPG cryptographic agent and passphrase cache (access for web browsers).
          10 %s %d %s %s %s[%d]: %s %s %s %s (%s %s).||Apr 29 08:08:44 ubuntu systemd[2014467]: Closed GnuPG cryptographic agent (ssh-agent emulation).
          10 %s %d %s %s %s[%d]: %s %s %s -%s %s %s||Apr 29 08:26:58 ubuntu org.freedesktop.thumbnails.Cache1[3001889]: Registered thumbnailer evince-thumbnailer -s %s %u %o
          10 %s %d %s %s %s[%d]: %s %s %s --%s %s %s %s||Apr 29 08:26:58 ubuntu org.freedesktop.thumbnails.Cache1[3001889]: Registered thumbnailer gnome-thumbnail-font --size %s %u %o
          10 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s (%s).||Apr 29 08:08:44 ubuntu systemd[2014467]: Closed GnuPG cryptographic agent and passphrase cache (restricted).
          10 %s %d %s %s %s[%d]: [%s] %s %s %s: /%s/%s/%s/%s/%d||Apr 29 00:23:13 ubuntu whoopsie[976]: [00:23:13] Found usable connection: /org/freedesktop/NetworkManager/ActiveConnection/1
          10 %s %d %s %s %s[%d]: [%s] %s %s %s %s %s: /%s/%s/%s/%s/%d||Apr 29 00:23:13 ubuntu whoopsie[976]: [00:23:13] The default IPv4 route is: /org/freedesktop/NetworkManager/ActiveConnection/1
          10 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s (%s %s %s %s).||Apr 29 08:08:44 ubuntu systemd[2014467]: Closed GnuPG cryptographic agent and passphrase cache (access for web browsers).
          10 %s %d %s %s %s[%d]: [%s] %s %x %s %s %s: /%s/%s/%s/%s/%d||Apr 29 00:23:13 ubuntu whoopsie[976]: [00:23:13] Not a paid data plan: /org/freedesktop/NetworkManager/ActiveConnection/1
          11 %s %d %s %s %s[%d]: %s %s:%s>'%s'||Apr 29 08:08:44 ubuntu tracker-miner-fs[2014475]: Received signal:15->'Terminated'
          11 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s||Apr 29 03:00:19 ubuntu systemd[1]: Failed to start Refresh fwupd metadata and update motd.
          11 %s %d %s %s %s[%d]: %s %s %s %s() %s: %s %s %s %s||Apr 29 08:08:44 ubuntu indicator-sound[2014629]: Error while sending AddMatch() message: The connection is closed
          12 %s %d %s %s %s[%d]: %s %s %s %s: %s %s %s: %s %s||Apr 29 08:26:58 ubuntu indicator-keyboard-service[3001919]: Unable to init server: Could not connect: Connection refused
          15 %s %d %s %s %s[%d]: (%s) %s (   %x / && %s --%s /%s/%s)||Apr 29 00:17:01 ubuntu CRON[2019319]: (root) CMD (   cd / && run-parts --report /etc/cron.hourly)
          18 %s %d %s %s %s[%d]: %s %s %s %d %s %s %d %s %s '%d' %s %s %s %f||Apr 29 08:26:57 ubuntu rtkit-daemon[1571]: Successfully made thread 3001837 of process 3001795 owned by '128' RT at priority 5.
          18 %s %d %s %s %s[%d]: %s %s & %s %s||Apr 29 08:26:58 ubuntu systemd[1]: Starting Time & Date Service...
          18 %s %d %s %s %s[%d]: %s %s %s %s %s %d||Apr 29 08:26:57 ubuntu tracker-extract[3001796]: Setting priority nice level to 19
          18 %s %d %s %s %s[%d]: %s %s %s %s %s %s %x %s||Apr 29 08:26:57 ubuntu pulseaudio[3001795]: Disabling timer-based scheduling because running inside a VM.
          19 %s %d %s %s %s[%d]: %s||Apr 29 08:08:44 ubuntu tracker-miner-fs[2014475]: OK
          19 %s %d %s %s %s[%d]: %s %s %s %s %s %f||Apr 29 08:08:44 ubuntu systemd[1]: Stopped User Manager for UID 128.
          19 %s %d %s %s %s[%d]: %s %s %s %s %s %s %f||Apr 29 08:08:44 ubuntu systemd[1]: Removed slice User Slice of UID 128.
          19 %s %d %s %s %s[%d]: %s %s %s %s /%s/%s/%f||Apr 29 08:08:44 ubuntu systemd[1]: Stopped User Runtime Directory /run/user/128.
          19 %s %d %s %s %s[%d]: %s %s %s %s /%s/%s/%s||Apr 29 08:08:44 ubuntu systemd[1]: Stopping User Runtime Directory /run/user/128...
          20 %s %d %s %s %s[%d]: <%s>  [%f] %s: %s %s %s %s %s||Apr 29 00:23:13 ubuntu NetworkManager[753]: <info>  [1619680993.1288] manager: NetworkManager state is now CONNECTED_SITE
          20 %s %d %s %s %s[%d]: [%s] %s||Apr 29 00:23:13 ubuntu whoopsie[976]: [00:23:13] offline
          29 %s %d %s %s %s[%d]: %s %s %s & %s %s||Apr 29 08:08:44 ubuntu systemd[2014467]: Stopping Indicator Date & Time Backend...
          29 %s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on GnuPG cryptographic agent and passphrase cache.
          29 %s %d %s %s %s[%d]: %s %s %x %s %s %s||Apr 29 08:08:34 ubuntu systemd[1]: Stopping Session c18 of user lightdm.
          30 %s %d %s %s %s[%d]: %s %s /%s/%s/%s -%s %s %s||Apr 29 08:26:58 ubuntu org.freedesktop.thumbnails.Cache1[3001889]: Registered thumbnailer /usr/bin/gdk-pixbuf-thumbnailer -s %s %u %o
          38 %s %d %s %s %s[%d]: %s %s: %s=:%f %s=/%s/%s/%s||Apr 29 08:08:44 ubuntu bluetoothd[745]: Endpoint unregistered: sender=:1.745 path=/MediaEndpoint/A2DPSink/sbc
          89 %s %d %s %s %s[%d]: %s %s %s %s %s||Apr 29 02:30:40 ubuntu systemd[1]: Starting Message of the Day...
          90 %s %d %s %s %s[%d]: [%s] %s %s %s '%s'||Apr 29 00:03:30 ubuntu dbus-daemon[752]: [system] Successfully activated service 'org.freedesktop.nm_dispatcher'
         103 %s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s '%s'||Apr 29 08:26:57 ubuntu dbus-daemon[3001807]: [session uid=128 pid=3001807] Successfully activated service 'org.gtk.vfs.Daemon'
         114 %s %d %s %s %s[%d]: %s %s %s %s - %s %s %s %s||Apr 29 08:08:44 ubuntu systemd[2014467]: Stopping Virtual filesystem service - Apple File Conduit monitor...
         116 %s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s  => '%d'||Apr 29 00:03:30 ubuntu NetworkManager[753]: <info>  [1619679810.1128] dhcp4 (ens33): option requested_host_name  => '1'
         116 %s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s          => '%i'||Apr 29 00:03:30 ubuntu NetworkManager[753]: <info>  [1619679810.1127] dhcp4 (ens33): option next_server          => '192.168.253.254'
         175 %s %d %s %s %s[%d]: %s %s %s||Apr 29 06:00:25 ubuntu systemd[1]: Starting PackageKit Daemon...
         225 %s %d %s %s %s[%d]: %s %s %s %s %s %s||Apr 29 00:03:30 ubuntu systemd[1]: Starting Network Manager Script Dispatcher Service...
         283 %s %d %s %s %s[%d]: %s %s %s %s||Apr 29 00:00:10 ubuntu systemd[1]: Finished Rotate log files.
         471 %s %d %s %s %s[%d]: %s: %s||Apr 29 00:00:10 ubuntu systemd[1]: logrotate.service: Succeeded.
         754 %s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s => '%d'||Apr 29 00:03:30 ubuntu NetworkManager[753]: <info>  [1619679810.1127] dhcp4 (ens33): option requested_broadcast_address => '1'
```

If the standard templating mode is obscuring too much information, you can switch to clustering mode (-c).  This reduces the parsing speed, but allows the templates to retain all of the non-variable strings.  I normally strip off the trailing example line when running in this mode just to keep the line length more managable.  Below is an example of the same log processed with the clustering (-c) parsing option.

```% tmpltr -c /var/log/secure.log | sort -n
Opening [/var/log/syslog] for read
           1 Apr 29 03:10:01 ubuntu CRON[2987654]: (root) CMD (test -e /run/systemd/system || SERVICE_MODE=1 /sbin/e2scrub_all -A -r)
           1 Apr 29 06:25:01 ubuntu CRON[2994686]: (root) CMD (test -x /usr/sbin/anacron || ( cd / && run-parts --report /etc/cron.daily ))
           1 Apr 29 07:30:35 ubuntu anacron[2996986]: Jobs will be executed sequentially
           1 Apr 29 07:30:35 ubuntu anacron[2996986]: Will run job `cron.daily' in 5 min.
           1 Apr 29 07:35:35 ubuntu anacron[2996986]: Job `cron.daily' started
           1 Apr 29 07:35:35 ubuntu anacron[2997066]: Updated timestamp for job `cron.daily' to 2021-04-29
           1 Apr 29 07:35:36 ubuntu anacron[2996986]: Job `cron.daily' terminated (exit status: 1) (mailing output)
           1 Apr 29 07:35:36 ubuntu cracklib: no dictionary update necessary.
           1 Apr 29 07:35:36 ubuntu postfix/local[2998370]: 7AD5A460101: to=<root@ubuntu-dev.uberadmin.com>, orig_to=<root>, relay=local, delay=0, delays=0/0/0/0, dsn=2.0.0, status=sent (delivered to mailbox)
           1 Apr 29 08:00:02 ubuntu crontab[3000587]: (root) LIST (nobody)
           1 Apr 29 08:08:44 ubuntu gvfsd[2014500]: A connection to the bus can't be made
           1 Apr 29 11:50:41 ubuntu fwupdmgr[3006998]: Successfully downloaded new metadata: 0 local devices supported
           1 Apr 29 12:50:34 ubuntu [3008013]: g_object_ref: assertion 'old_val > 0' failed
           2 Apr 29 %s ubuntu indicator-appli[%d]: Name Lost||Apr 29 08:08:44 ubuntu indicator-appli[2014597]: Name Lost
           2 Apr 29 %s ubuntu indicator-datet[%d]: indicator-datetime exiting; failed/lost bus ownership||Apr 29 08:08:44 ubuntu indicator-datet[2014612]: indicator-datetime exiting; failed/lost bus ownership
           3 Apr 29 09:31:32 ubuntu tumblerd[3003112]: Name %s lost on the message dbus, exiting.||Apr 29 09:31:32 ubuntu tumblerd[3003112]: Name org.freedesktop.thumbnails.Cache1 lost on the message dbus, exiting.
           3 Apr 29 %s ubuntu postfix/cleanup[%d]: %x: message-id=<%s>||Apr 29 01:08:00 ubuntu postfix/cleanup[2984439]: 119904600DA: message-id=<20210429080800.119904600DA@ubuntu.localdomain>
           3 Apr 29 %s ubuntu postfix/pickup[%d]: %x: uid=0 from=<root>||Apr 29 01:08:00 ubuntu postfix/pickup[2015577]: 119904600DA: uid=0 from=<root>
           3 Apr 29 %s ubuntu postfix/qmgr[2145]: %x: removed||Apr 29 01:08:00 ubuntu postfix/qmgr[2145]: 119904600DA: removed
           3 Apr 29 %s ubuntu snapd[791]: autorefresh.go:479: auto-refresh: all snaps are up-to-date||Apr 29 01:33:32 ubuntu snapd[791]: autorefresh.go:479: auto-refresh: all snaps are up-to-date
           3 Apr 29 %s ubuntu xfce4-notifyd[%d]: cannot open display: ||Apr 29 08:42:57 ubuntu xfce4-notifyd[3002212]: cannot open display: 
           4 Apr 29 %s ubuntu fwupdmgr[%d]: Fetching %s https://cdn.fwupd.org/downloads/%s||Apr 29 03:00:19 ubuntu fwupdmgr[2987489]: Fetching metadata https://cdn.fwupd.org/downloads/firmware.xml.gz
           4 Apr 29 %s ubuntu PackageKit: daemon %s||Apr 29 06:00:25 ubuntu PackageKit: daemon start
           4 Apr 29 %s ubuntu systemd[%d]: %s: Failed with result 'exit-code'.||Apr 29 03:00:19 ubuntu systemd[1]: fwupd-refresh.service: Failed with result 'exit-code'.
           4 Apr 29 %s ubuntu systemd[%d]: %s: Main process exited, code=exited, status=1/FAILURE||Apr 29 03:00:19 ubuntu systemd[1]: fwupd-refresh.service: Main process exited, code=exited, status=1/FAILURE
           5 Apr 29 %s ubuntu systemd[%d]: dbus.service: Killing process %d (%s) with signal SIGKILL.||Apr 29 08:08:44 ubuntu systemd[2014467]: dbus.service: Killing process 2014538 (gdbus) with signal SIGKILL.
           7 Apr 29 %s ubuntu anacron[%d]: Anacron %f 2.3 started on||Apr 29 07:30:35 ubuntu anacron[2996986]: Anacron 2.3 started on 2021-04-29
           7 Apr 29 %s ubuntu anacron[%d]: Normal exit (%d %s run)||Apr 29 07:35:36 ubuntu anacron[2996986]: Normal exit (1 job run)
           9 Apr 29 %s ubuntu dbus-daemon[%d]: [session uid=128 pid=%d] AppArmor D-Bus mediation is enabled||Apr 29 08:26:57 ubuntu dbus-daemon[3001807]: [session uid=128 pid=3001807] AppArmor D-Bus mediation is enabled
           9 Apr 29 %s ubuntu indicator-sound[%d]: g_object_ref: assertion 'old_val > 0' failed||Apr 29 08:08:44 ubuntu indicator-sound[2014629]: g_object_ref: assertion 'old_val > 0' failed
           9 Apr 29 %s ubuntu systemd[%d]: Listening on GnuPG cryptographic agent (ssh-agent emulation).||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on GnuPG cryptographic agent (ssh-agent emulation).
           9 Apr 29 %s ubuntu systemd[%d]: Listening on REST API socket for snapd user session agent.||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on REST API socket for snapd user session agent.
          10 Apr 29 %s ubuntu %s[%d]: Registered thumbnailer evince-thumbnailer -s %s %u %o||Apr 29 08:26:58 ubuntu org.freedesktop.thumbnails.Cache1[3001889]: Registered thumbnailer evince-thumbnailer -s %s %u %o
          10 Apr 29 %s ubuntu systemd[%d]: Closed GnuPG cryptographic agent and passphrase cache (restricted).||Apr 29 08:08:44 ubuntu systemd[2014467]: Closed GnuPG cryptographic agent and passphrase cache (restricted).
          10 Apr 29 %s ubuntu systemd[%d]: Closed GnuPG cryptographic agent (ssh-agent emulation).||Apr 29 08:08:44 ubuntu systemd[2014467]: Closed GnuPG cryptographic agent (ssh-agent emulation).
          11 Apr 29 %s ubuntu %s[%d]: Error while sending AddMatch() message: The connection is closed||Apr 29 08:08:44 ubuntu indicator-sound[2014629]: Error while sending AddMatch() message: The connection is closed
          11 Apr 29 %s ubuntu %s[%d]: Received signal:15->'Terminated'||Apr 29 08:08:44 ubuntu tracker-miner-fs[2014475]: Received signal:15->'Terminated'
          11 Apr 29 %s ubuntu systemd[%d]: %s %s %s %s %s %s %s %s %s||Apr 29 03:00:19 ubuntu systemd[1]: Failed to start Refresh fwupd metadata and update motd.
          12 Apr 29 %s ubuntu %s[%d]: Unable to init server: Could not connect: Connection refused||Apr 29 08:26:58 ubuntu indicator-keyboard-service[3001919]: Unable to init server: Could not connect: Connection refused
          15 Apr 29 %s ubuntu CRON[%d]: (root) CMD (   cd / && run-parts --report /etc/cron.hourly)||Apr 29 00:17:01 ubuntu CRON[2019319]: (root) CMD (   cd / && run-parts --report /etc/cron.hourly)
          18 Apr 29 %s ubuntu pulseaudio[%d]: Disabling timer-based scheduling because running inside a VM.||Apr 29 08:26:57 ubuntu pulseaudio[3001795]: Disabling timer-based scheduling because running inside a VM.
          18 Apr 29 %s ubuntu %s[%d]: Setting priority nice level to 19||Apr 29 08:26:57 ubuntu tracker-extract[3001796]: Setting priority nice level to 19
          18 Apr 29 %s ubuntu systemd[1]: %s Time & Date %s||Apr 29 08:26:58 ubuntu systemd[1]: Starting Time & Date Service...
          19 Apr 29 %s ubuntu %s[%d]: OK||Apr 29 08:08:44 ubuntu tracker-miner-fs[2014475]: OK
          19 Apr 29 %s ubuntu systemd[1]: %s slice User Slice of UID %f||Apr 29 08:08:44 ubuntu systemd[1]: Removed slice User Slice of UID 128.
          19 Apr 29 %s ubuntu systemd[1]: %s User Manager for UID %f||Apr 29 08:08:44 ubuntu systemd[1]: Stopped User Manager for UID 128.
          19 Apr 29 %s ubuntu systemd[1]: %s User Runtime Directory /run/user/128...||Apr 29 08:08:44 ubuntu systemd[1]: Stopping User Runtime Directory /run/user/128...
          19 Apr 29 %s ubuntu systemd[1]: %s User Runtime Directory /run/user/%f||Apr 29 08:08:44 ubuntu systemd[1]: Stopped User Runtime Directory /run/user/128.
          20 Apr 29 %s ubuntu whoopsie[976]: [%s] %s||Apr 29 00:23:13 ubuntu whoopsie[976]: [00:23:13] offline
          29 Apr 29 %s ubuntu systemd[1]: %s Session %x of user lightdm.||Apr 29 08:08:34 ubuntu systemd[1]: Stopping Session c18 of user lightdm.
          29 Apr 29 %s ubuntu systemd[%d]: %s Indicator Date & Time %s||Apr 29 08:08:44 ubuntu systemd[2014467]: Stopping Indicator Date & Time Backend...
          29 Apr 29 %s ubuntu systemd[%d]: %s %s %s %s %s and %s %s||Apr 29 08:26:57 ubuntu systemd[3001787]: Listening on GnuPG cryptographic agent and passphrase cache.
          30 Apr 29 %s ubuntu %s[%d]: Registered thumbnailer /usr/bin/%s -s %s %u %o||Apr 29 08:26:58 ubuntu org.freedesktop.thumbnails.Cache1[3001889]: Registered thumbnailer /usr/bin/gdk-pixbuf-thumbnailer -s %s %u %o
          38 Apr 29 %s ubuntu bluetoothd[745]: Endpoint %s: sender=:%f %s=/path/MediaEndpoint/%s||Apr 29 08:08:44 ubuntu bluetoothd[745]: Endpoint unregistered: sender=:1.745 path=/MediaEndpoint/A2DPSink/sbc
          45 Apr 29 %s ubuntu rtkit-daemon[1571]: Supervising %d threads of 2 processes of 2 users.||Apr 29 08:26:57 ubuntu rtkit-daemon[1571]: Supervising 4 threads of 2 processes of 2 users.
          52 Apr 29 %s ubuntu systemd[%d]: %s %s %s %s %s %s %s||Apr 29 03:00:19 ubuntu systemd[1]: Starting Refresh fwupd metadata and update motd...
          76 Apr 29 %s ubuntu systemd[%d]: %s Virtual filesystem service - %s %s %s||Apr 29 08:08:44 ubuntu systemd[2014467]: Stopping Virtual filesystem service - digital camera monitor...
          89 Apr 29 %s ubuntu %s[%d]: %s %s %s %s %s||Apr 29 02:30:40 ubuntu systemd[1]: Starting Message of the Day...
          90 Apr 29 %s ubuntu dbus-daemon[752]: [system] Successfully activated service '%s'||Apr 29 00:03:30 ubuntu dbus-daemon[752]: [system] Successfully activated service 'org.freedesktop.nm_dispatcher'
         114 Apr 29 %s ubuntu systemd[%d]: %s Virtual filesystem service - %s %s %s %s||Apr 29 08:08:44 ubuntu systemd[2014467]: Stopping Virtual filesystem service - Apple File Conduit monitor...
         116 Apr 29 %s ubuntu NetworkManager[753]: <info>  [%f] %s (dhcp4): ens33 option  => '%d'||Apr 29 00:03:30 ubuntu NetworkManager[753]: <info>  [1619679810.1128] dhcp4 (ens33): option requested_host_name  => '1'
         175 Apr 29 %s ubuntu %s[%d]: %s %s %s||Apr 29 06:00:25 ubuntu systemd[1]: Starting PackageKit Daemon...
         225 Apr 29 %s ubuntu systemd[%d]: %s %s %s %s %s %s||Apr 29 00:03:30 ubuntu systemd[1]: Starting Network Manager Script Dispatcher Service...
         283 Apr 29 %s ubuntu systemd[%d]: %s %s %s %s||Apr 29 00:00:10 ubuntu systemd[1]: Finished Rotate log files.
         471 Apr 29 %s ubuntu systemd[%d]: %s: Succeeded.||Apr 29 00:00:10 ubuntu systemd[1]: logrotate.service: Succeeded.
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
% tmpltr -w ignore.templates /var/log/syslog | sort -n
```

After the above run of the tool, a file named ignore.templates
is created in the current directory as shown below:

```$ cat ignore.templates
%s %d %s %s %s[%d]: %s: %s %s'%s, %s %s %s %s, %s
%s %d %s %s %s[%d]: %s %s %s `%s' %s %d %s
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s    => '%d'
%s %d %s %s %s[%d]: %s:%d: %s %s %s %s %s %s %s '%s:%s=/%s/%s/%d/%s/%s': %s %s %s: %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s (%s %s %s %s).
%s %d %s %s %s[%d]: %c_%s: %s '%s > %d' %s
%s %d %s %s %s[%d]: (%s) %s (%s -%s/%s/%s/%s || ( %x / && %s --%s /%s/%s ))
%s %d %s %s %s/%s[%d]: %x: %s=<%s>, %s=<%s>, %s=%s, %s=%f, %s=%f/%d/%d/%d, %s=%s %s=%s (%s %s %s)
%s %d %s %s %s[%d]: %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %x %s
%s %d %s %s [%d]: %c_%s: %s '%s > %d' %s
%s %d %s %s %s[%d]: %s %s %s %s: %s %s %s: %s %s
%s %d %s %s %s[%d]: %s %s %s %s (%s %s).
%s %d %s %s %s[%d]: %s %s %x %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s `%s' %s %s
%s %d %s %s %s[%d]: [%s] %s %s %s '%s'
%s %d %s %s %s[%d]: <%s>  [%f] %s: %s %s %s %s %s
%s %d %s %s %s[%d]: %s: %s %s %s '%s'.
%s %d %s %s %s[%d]: %s %s %s://%s/%s/%s
%s %d %s %s %s[%d]: %s %s & %s %s
%s %d %s %s %s[%d]: <%s>  [%f] %s: %s[%x,:%f/%s/%d]: %s %s
%s %d %s %s %s[%d]: %s %d %s %s %d %s %s %d %s
%s %d %s %s %s[%d]: %s %f %s %s %s
%s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s '%x.%s'
%s %d %s %s %s[%d]: [%s] %s %s %s: /%s/%s/%s/%s/%d
%s %d %s %s %s[%d]: %s[%d]: %s %s %s '%s'
%s %d %s %s %s[%d]: %s %s %s: 
%s %d %s %s %s[%d]: %s: %s
%s %d %s %s %s[%d]: %s %s %s & %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s %s %s, %s %s %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s; %s/%s %s %s
%s %d %s %s %s[%d]: %s %s %s %d %s %s %d %s %s '%d' %s %s %s %s %s -%f
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s %s &%s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s - %s
%s %d %s %s %s[%d]: %s:%s %s %s %s %s %s: %s:%s: %s %s %s %s %s %s %s %s .%s %s
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s      => '%d'
%s %d %s %s %s[%d]: (%s) %s (%s)
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s -%s %s %s
%s %d %s %s %s[%d]: (%s) %s ([ -%s/%s/%s/%s ] && %s [ ! -%x /%s/%s/%s ]; %s /%s/%s/%s %s %s >/%s/%s; %s)
%s %d %s %s %s[%d]: %s: %s %s'%s %s %s %s %s %s
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s => '%d'
%s %d %s %s %s[%d]: %s %s %s --%s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s (%s).
%s %d %s %s %s[%d]: %s: %s '%s (%s)' %s
%s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s '%s'
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s; %s %s/%s/%s
%s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s %s %s
%s %d %s %s %s[%d]: (%s) %s (   %x / && %s --%s /%s/%s)
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %x %s %s %s %s %s '%s'. %s %s %s %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s /%s/%s/%s -%s %s %s
%s %d %s %s %s[%d]: %s
%s %d %s %s %s[%d]: %s %s %s %s
%s %d %s %s %s[%d]: %s %s %x %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %d %s %s %d %s %s '%d' %s %s %s %f
%s %d %s %s %s[%d]: %s[%d]: %s %s %s='%s' %s %s ':%f' (%s=%d %s=%d %s="%s" %s="%s")
%s %d %s %s %s[%d]: %s %s: %s=:%f %s=/%s/%s/%s
%s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s: %s %s='%s' %s='%s' %s %s ':%f' (%s=%d %s=%d %s="%s" %s="%s")
%s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s='%x.%s' %s %s ':%f' (%s=%d %s=%d %s="%s" %s="%s")
%s %d %s %s %s[%d]: %s %s %s %s - %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s: %d %s %s %s
%s %d %s %s %s[%d]: %s:%d: %s: %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s (%s).
%s %d %s %s %s[%d]: %s `%s' %s
%s %d %s %s %s[%d]: %s %s:%s>'%s'
%s %d %s %s %s[%d]: %x %s %s %s %s %s'%s %s
%s %d %s %s %s[%d]: %s %s %d %s: [ %s %s %s %s() %s: %s %s %s %s]
%s %d %s %s %s[%d]: %s:%d: %s %s: %s %s %s %s %s: "%s", "%s", "%s", "%s", "%s"
%s %d %s %s %s[%d]: %s %s %s %s %s (%s %s).
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s              => '%i'
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s  => '%i'
%s %d %s %s %s[%d]: %s %s %s %s - %s %s %s %s
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s          => '%i'
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s  => '%d'
%s %d %s %s %s[%d]: [%s] %s %s %s %s %s: /%s/%s/%s/%s/%d
%s %d %s %s %s/%s[%d]: %x: %s=%d %s=<%s>
%s %d %s %s %s[%d]: %s: %s %s %s, %s=%s, %s=%d/%s
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s       => '%d'
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s: %s
%s %d %s %s %s[%d]: [%s] %s %x %s %s %s: /%s/%s/%s/%s/%d
%s %d %s %s %s[%d]: (%s) %s (   %s -%s/%s/%s/%s && { [ -%s" ] && . "%s" ; %s -%c%s /%s/%s/%s -%s; })
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s, %s
%s %d %s %s %s[%d]: %s %s %s %s: %s:%s: %s %s %s %s
%s %d %s %s %s[%d]: [%s %s=%d %s=%d] %s %s %s='%s' %s %s ':%f' (%s=%d %s=%d %s="%s" %s="%s")
%s %d %s %s %s[%d]: %s %s %s %s %d %s %s %s %s %f
%s %d %s %s %s[%d]: %s: %s %s %d (%s) %s %s %s
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s           => '%i'
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s               => '%d'
%s %d %s %s %s/%s[%d]: %x: %s=<%s>, %s=<%s>, %s=%s, %s=%d, %s=%d/%d/%d/%d, %s=%s %s=%s (%s %s %s)
%s %d %s %s %s: %s %s
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s          => '%s'
%s %d %s %s %s[%d]: %s %s %s %s %s %f
%s %d %s %s %s[%d]: (%s) %s (%s -%x /%s/%s/%s || %s=%d /%s/%s -%x -%c)
%s %d %s %s %s: %s %s %s %s
%s %d %s %s %s/%s[%d]: %x: %s=<%s>, %s=%d, %s=%d (%s %s)
%s %d %s %s %s[%d]: [%s] %s
%s %d %s %s %s[%d]: [%s] %s %s %s: %s %s='%s' %s='%s' %s %s ':%f' (%s=%d %s=%d %s="%s" %s="%s")
%s %d %s %s %s[%d]: %s %s (%d %s %s)
%s %d %s %s %s[%d]: %s %s %s %s /%s/%s/%f
%s %d %s %s %s[%d]: <%s>  [%f] %s (%s): %s %s %s -> %s
%s %d %s %s %s/%s[%d]: %x: %s
%s %d %s %s %s[%d]: %s `%s' %s (%s %s: %d) (%s %s)
%s %d %s %s %s[%d]: %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s (%s %s %s %s).
%s %d %s %s %s[%d]: %s %s %s %s() %s: %s %s %s %s
%s %d %s %s %s[%d]: %s %s
%s %d %s %s %s[%d]: %s %s %s %s /%s/%s/%s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %f
%s %d %s %s %s/%s[%d]: %x: %s=<%s>
%s %d %s %s %s[%d]: %s %s %s %s %s %d
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s %s %s %s
%s %d %s %s %s[%d]: %s %s %s %s %s %s %s -- %s %x %s %s() %s %d %s %s %s < %s
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
