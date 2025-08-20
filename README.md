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
**37M** lines per minute on a 3GHz x86 machine running *NIX.

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
tmpltr v1.0.7 [Aug 20 2025 - 16:12:01]

syntax: tmpltr [options] filename [filename ...]
 -c|--cluster           show invariable fields in output
 -d|--debug (0-9)       enable debugging info
 -g|--greedy            ignore quotes
 -h|--help              this info
 -l|--line {line}       show all lines that match template of {line}
 -L|--linefile {fname}  show all the lines that match templates of lines in {fname}
 -m|--match {template}  show all lines that match {template}
 -M|--matchfile {fname} show all the lines that match templates in {fname}
 -n|--cnum {num}        max cluster args [default: 2]
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
frequency of occurrence from least to most prevalent.

I normally go through the list in this order to speed up
catching the anonymous patterns.

```
% tmpltr /var/log/syslog | sort -n
           1 %D %s %s: %s %s: %s %s %s %s %s, %s, %s %s %s||Aug 19 22:28:51 hyprframe kernel: rtc_cmos rtc_cmos: alarms up to one month, y3k, 114 bytes nvram
           1 %D %s %s: %s: `%s' %s %s %s %s %s %s %s %s %s %d %s; %s %s||Aug 19 22:28:51 hyprframe kernel: warning: `ThreadPoolForeg' uses wireless extensions which will stop working for Wi-Fi 7 hardware; use nl80211
           1 %D %s %s: %s %s: [%s] %s %s (%s %s %s) %s %s %s %f %s %s||Aug 19 22:28:51 hyprframe kernel: i915 0000:00:02.0: [drm] Found tigerlake/uy (device ID 9a49) integrated display version 12.00 stepping C0
           1 %D %s %s: %s %s %s %s %s %s %s %s %s (%s %s): %s||Aug 19 22:28:51 hyprframe kernel: Estimated ratio of average max frequency by base frequency (times 1024): 1467
           1 %D %s %s: %s: %s: %s %s [%s:%s,%s:%s] %s %s,%s %s %d,%s||Aug 19 22:28:51 hyprframe kernel: i8042: PNP: PS/2 Controller [PNP0303:PS2K,PNP0f03:PS2M] at 0x60,0x64 irq 1,12
           1 %D %s %s: %s %s:%s _%s: %s %s %s [%s %s %s %s %s %s]||Aug 19 22:28:51 hyprframe kernel: acpi PNP0A08:00: _OSC: OS now controls [PCIeHotplug SHPCHotplug PME PCIeCapability LTR DPC]
           1 %D %s %s: %s %s:%s _%s: %s %s [%s %s %s %s %s %s %s]||Aug 19 22:28:51 hyprframe kernel: acpi PNP0A08:00: _OSC: OS supports [ExtendedConfig ASPM ClockPM Segments MSI EDR HPX-Type3]
           1 %D %s %s: %s: %s %s %s %s, %s %s %s %s %s, %s '%s' %s.||Aug 19 22:28:51 hyprframe kernel: x86/fpu: Enabled xstate features 0xae7, context size is 2456 bytes, using 'compacted' format.
           1 %D %s %s: %s: %s %s (%s %s %s, %s %s, %s %s, %s %s, %s %s, %s %s, %s %x-%s)||Aug 19 22:28:51 hyprframe kernel: Memory: 65552672K/66886968K available (19713K kernel code, 2940K rwdata, 16528K rodata, 4644K init, 5024K bss, 1301140K reserved, 0K cma-reserved)
           1 %D %s %s: %s: %s %s %s %s %s %s; %s %s, %s "%s" %s %s %x %s||Aug 19 22:28:51 hyprframe kernel: PCI: Using host bridge windows from ACPI; if necessary, use "pci=nocrs" and report a bug
           1 %D %s %s: %s: %s %s %s %s (%s = %s, %s_%s = %s) %s %x %s||Aug 19 22:28:51 hyprframe kernel: 0000:00:16.3: ttyS4 at I/O 0x3060 (irq = 19, base_baud = 115200) is a 16550A
           1 %D %s %s: %s %s: [%s] %s %s %s %s %s %s %s %x||Aug 19 22:28:51 hyprframe kernel: i915 0000:00:02.0: [drm] Selective fetch area calculation failed in pipe A
           1 %D %s %s: %s %s: %s,%s: %s %s %s %s [%x:%s %s] %s %s:%s||Aug 19 22:28:51 hyprframe kernel: hid-generic 0018:32AC:0006.0003: input,hidraw2: I2C HID v1.00 Device [FRMW0001:00 32AC:0006] on i2c-FRMW0001:00
           1 %D %s %s: %s %s %s %s %s  %s:%s %x:%d %x:%s %s||Aug 19 22:28:51 hyprframe kernel: Adding 4194300k swap on /dev/zram0.  Priority:100 extents:1 across:4194300k SSDsc
           1 %D %s %s: %s %s %s %s %s %x: %s||Aug 19 22:28:51 hyprframe kernel: Write protecting the kernel read-only data: 38912k
           1 %D %s %s: %s: %s %s %s %s (%x) %s %s||Aug 19 22:28:51 hyprframe kernel: hid: raw HID events driver (C) Jiri Kosina
           1 %D %s %s: %s %s: %s: %s %s %x: %s=%s,%s=%s:%s=%s||Aug 19 22:28:51 hyprframe kernel: i915 0000:00:02.0: vgaarb: VGA decodes changed: olddecodes=io+mem,decodes=io+mem:owns=io+mem
           1 %D %s %s: %s %s: %s: %s %s %x: %s=%s,%s=%s,%s=%s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: vgaarb: VGA device added: decodes=io+mem,owns=io+mem,locks=none
           1 %D %s %s: %s: %s: %s %s %x:||Aug 19 22:28:51 hyprframe kernel: smpboot: x86: Booting SMP configuration:
           1 %D %s %s: %s: %s %s: %s, %x: %d-%s||Aug 19 22:28:51 hyprframe kernel: PM: RTC time: 05:25:11, date: 2025-08-20
           1 %D %s %s: %s %s: %s %s %x: %m||Aug 19 22:28:51 hyprframe kernel: iwlwifi 0000:aa:00.0: base HW address: 8c:b8:7e:c8:bf:f4
           1 %D %s %s: %s %s: %s %s %x: %s, %s %s: %d||Aug 19 22:28:51 hyprframe kernel: iwlwifi 0000:aa:00.0: Registered PHC clock: iwlwifi-PTP, with index: 0
           1 %D %s %s: %s %s: %s %s %x-%s %s %s, %s||Aug 19 22:28:51 hyprframe kernel: ee1004 17-0050: 512 byte EE1004-compliant SPD EEPROM, read-only
           1 %D %s %s: %s %s %s %s %x: %s (%s: %s, %s %s, %s)||Aug 19 22:28:51 hyprframe kernel: TCP established hash table entries: 524288 (order: 10, 4194304 bytes, linear)
           1 %D %s %s: %s: %s %s %x: %s||Aug 19 22:28:51 hyprframe kernel: microcode: Updated early from: 0x00000086
           1 %D %s %s: %s: %s %s %x: %s [%s %d-%s], %s %s %s %s %s %s [%s %d-%s]||Aug 19 22:28:51 hyprframe kernel: resource: resource sanity check: requesting [mem 0x00000000fedc0000-0x00000000fedcdfff], which spans more than pnp 00:02 [mem 0xfedc0000-0xfedc7fff]
           1 %D %s %s: %s %s: %s %x %s %s %s %s (%s=%d, %s=%s)||Aug 19 22:28:51 hyprframe kernel: iTCO_wdt iTCO_wdt: Found a Intel PCH TCO device (Version=6, TCOBASE=0x0400)
           1 %D %s %s: %s %s: %s %x-%s %s, %x-%s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: iwlwifi 0000:aa:00.0: Detected crf-id 0x400410, cnv-id 0x400410 wfpm id 0x80000000
           1 %D %s %s: %s %s %x: %s||Aug 19 22:28:51 hyprframe kernel: IPI shorthand broadcast: enabled
           1 %D %s %s: %s: %s %x: %s||Aug 19 22:28:51 hyprframe kernel: zram: Added device: zram0
           1 %D %s %s: %s: %s %x: %s %d %s||Aug 19 22:28:51 hyprframe kernel: pnp: PnP ACPI: found 6 devices
           1 %D %s %s: %s %s %x-%s %s||Aug 19 22:28:51 hyprframe kernel: Key type fscrypt-provisioning registered
           1 %D %s %s: %s %s: %x: %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: DMAR: Skip IOMMU disabling for graphics
           1 %D %s %s: %s %x-%s %c.%s %s||Aug 19 22:28:51 hyprframe kernel: Loading compiled-in X.509 certificates
           1 %D %s %s: %s: %x: %s %s: %s||Aug 19 22:28:51 hyprframe kernel: pid_max: default: 32768 minimum: 301
           1 %D %s %s: %s %x: %s %s %s||Aug 19 22:28:51 hyprframe kernel: pinctrl core: initialized pinctrl subsystem
           1 %D %s %s: %s: %x: %s %s %s(%c) %s(%s) %s %c %f%s (%x: %s, %s: %s, %s: %s)||Aug 19 22:28:51 hyprframe kernel: smpboot: CPU0: 11th Gen Intel(R) Core(TM) i7-1185G7 @ 3.00GHz (family: 0x6, model: 0x8c, stepping: 0x1)
           1 %D %s %s: %s: %x:%s %s %s %s %s %b:%d:%s%s%s%s:%d:%s%s||Aug 19 22:28:51 hyprframe kernel: input: FRMW0001:00 32AC:0006 Consumer Control as /devices/pci0000:00/0000:00:15.1/i2c_designware.1/i2c-15/i2c-FRMW0001:00/0018:32AC:0006.0003/input/input9
           1 %D %s %s: %s: %x:%s %s %s %s %s %s %b:%d:%s%s%s%s:%d:%s%s||Aug 19 22:28:51 hyprframe kernel: input: FRMW0001:00 32AC:0006 Wireless Radio Control as /devices/pci0000:00/0000:00:15.1/i2c_designware.1/i2c-15/i2c-FRMW0001:00/0018:32AC:0006.0003/input/input8
           1 %D %s %s: %s: %x-%s %s %s %x: %s (%s %d, %s %s)||Aug 19 22:28:51 hyprframe kernel: VFS: Dquot-cache hash table entries: 512 (order 0, 4096 bytes)
           1 %D %s %s: %s %x: %s %x-%s,  %s %s, %s %s, %s %s, %x-%s %s, %s %s %s.||Aug 19 22:28:51 hyprframe kernel: Performance Events: PEBS fmt4+-baseline,  AnyThread deprecated, Icelake events, 32-deep LBR, full-width counters, Intel PMU driver.
           1 %D %s %s: %x: %c_%s: %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: \_SB_.PC00.LPCB.EC0_: Boot ECDT EC initialization complete
           1 %D %s %s: %x: %c_%s: %s %s _%s %s (%s: %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: \_SB_.PEPD: Duplicate LPS0 _DSM functions (mask: 0x79)
           1 %D %s %s: %x: %c_%s: %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: \_SB_.PC00.LPCB.EC0_: EC: Used to handle transactions and events
           1 %D %s %s: %x: %s||Aug 19 22:28:51 hyprframe kernel: devtmpfs: initialized
           1 %D %s %s: %x: %s(%c) %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: DMAR: Intel(R) Virtualization Technology for Directed I/O
           1 %D %s %s: %x: %s %d %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: Enabled 7 GPEs in block 00 to 7F
           1 %D %s %s: %x: %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: Interpreter enabled
           1 %D %s %s: %x: %s %s %b %s %s %s %s %s %s. %s %s %s %s %s %s_%s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: bridge: filtering via arp/ip/ip6tables is no longer available by default. Update your scripts to load br_netfilter if you need this.
           1 %D %s %s: %x: %s %s=%s||Aug 19 22:28:51 hyprframe kernel: ACPI: EC: GPE=0x6e
           1 %D %s %s: %x-%s: %s: %s ||Aug 19 22:28:51 hyprframe kernel: device-mapper: uevent: version 1.0.3
           1 %D %s %s: %x: %s (%s %s %f)||Aug 19 22:28:51 hyprframe kernel: fuse: init (API version 7.43)
           1 %D %s %s: %x: %s: %s %s %f %s %d %s %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Firmware timestamp 2025.13 buildtype 1 build 82008
           1 %D %s %s: %x: %s %s %s (%s)||Aug 19 22:28:51 hyprframe kernel: audit: initializing netlink subsys (disabled)
           1 %D %s %s: %x: %s (%s %s) %s %f||Aug 19 22:28:51 hyprframe kernel: Bluetooth: BNEP (Ethernet Emulation) ver 1.3
           1 %D %s %s: %x: %s: %s %s %s: %i||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Fseq BT Top: 00.00.02.41
           1 %D %s %s: %x: %s %s %s [%s] (%s)||Aug 19 22:28:51 hyprframe kernel: ACPI: AC: AC Adapter [ACAD] (on-line)
           1 %D %s %s: %x: %s _%s(%s %s %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: Added _OSI(Processor Aggregator Device)
           1 %D %s %s: %x: %s %s %s_%s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: bus type drm_connector registered
           1 %D %s %s: %x: %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: Using IOAPIC for interrupt routing
           1 %D %s %s: %x: %s: %s %s: %s (%s)||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Fseq status: Success (0x00)
           1 %D %s %s: %x: %s (%s) %s %s %s||Aug 19 22:28:51 hyprframe kernel: fbcon: i915drmfb (fb0) is primary device
           1 %D %s %s: %x: %s: (%s %s %s %s %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: PM: (supports S0 S3 S4 S5)
           1 %D %s %s: %x: %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: HCI device and connection manager initialized
           1 %D %s %s: %x-%s: %s: %s (%s) %s: %s||Aug 19 22:28:51 hyprframe kernel: device-mapper: ioctl: 4.49.0-ioctl (2025-01-17) initialised: dm-devel@lists.linux.dev
           1 %D %s %s: %x: %s: %s: %s %s: %s, %s: %b %s||Aug 19 22:28:51 hyprframe kernel: clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 1911260446275000 ns
           1 %D %s %s: %x: %s: %s %s %s %s [%s %d-%s] (%s %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: PM: Registering ACPI NVS region [mem 0x452ff000-0x45b2efff] (8585216 bytes)
           1 %D %s %s: %x: %s: [%s %s]: %s %s %s (-%s)||Aug 19 22:28:51 hyprframe kernel: ACPI: thermal: [Firmware Bug]: Invalid critical threshold (-274000)
           1 %D %s %s: %x: %s: [%s %s]: %s %s %s %s!||Aug 19 22:28:51 hyprframe kernel: ACPI: thermal: [Firmware Bug]: No valid trip points!
           1 %D %s %s: %x: %s: %s %s %s _%s %s %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: No support for _PRR ACPI method
           1 %D %s %s: %x: %s: %s %s %s %s %s (-%s)||Aug 20 11:55:39 hyprframe kernel: Bluetooth: hci0: Retrieving Intel exception info failed (-16)
           1 %D %s %s: %x: %s %s %s %s %s %s %s: %f||Aug 19 22:28:51 hyprframe kernel: acpiphp: ACPI Hot Plug PCI Controller Driver version: 0.5
           1 %D %s %s: %x: %s %s %s [%s] (%s %s [%s %s])||Aug 19 22:28:51 hyprframe kernel: ACPI: PCI Root Bridge [PC00] (domain 0000 [bus 00-fe])
           1 %D %s %s: %x: %s: %s: %s %s: %s, %s: %s %s||Aug 19 22:28:51 hyprframe kernel: clocksource: tsc: mask: 0xffffffffffffffff max_cycles: 0x2b2c8ec87c7, max_idle_ns: 440795278598 ns
           1 %D %s %s: %x: %s=%s %s(%s): %s=%s %s_%s=%d %s=%d||Aug 19 22:28:51 hyprframe kernel: audit: type=2000 audit(1755667511.010:1): state=initialized audit_enabled=0 res=1
           1 %D %s %s: %x: %s_%s: %s: %s %s: %s, %s: %s %s||Aug 19 22:28:51 hyprframe kernel: clocksource: acpi_pm: mask: 0xffffff max_cycles: 0xffffff, max_idle_ns: 2085701024 ns
           1 %D %s %s: %x: %s: %s %s [%s] (%s: %s  %s: %s  %s: %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: video: Video Device [GFX0] (multi-head: yes  rom: no  post: no)
           1 %D %s %s: %x-%s (%s): %s %s %s %s %s %s %s %s %s %s. %s %s %s.||Aug 19 22:28:51 hyprframe kernel: FAT-fs (nvme0n1p1): Volume was not properly unmounted. Some data may be corrupt. Please run fsck.
           1 %D %s %s: %x: %s: %s %s [%s] (%s %x)||Aug 19 22:28:51 hyprframe kernel: ACPI: thermal: Thermal Zone [TZ00] (50 C)
           1 %D %s %s: %x-%s (%s): %s %s %x-%s %s %s %s %s %s %s %s: %s||Aug 19 22:28:51 hyprframe kernel: EXT4-fs (dm-0): mounted filesystem f3457bf0-7448-405b-99fb-0346b62c0bf5 r/w with ordered data mode. Quota mode: none.
           1 %D %s %s: %x: %s: %s %s %x: %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Found device firmware: intel/ibt-0041-0041.sfi
           1 %D %s %s: %x-%s (%s): %s %x-%s||Aug 19 22:28:51 hyprframe kernel: EXT4-fs (dm-0): re-mounted f3457bf0-7448-405b-99fb-0346b62c0bf5.
           1 %D %s %s: %x: %s: %s %x: %i||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Fseq executed: 00.00.02.41
           1 %D %s %s: %x: %s: %s %x: %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Boot Address: 0x100800
           1 %D %s %s: ... %x-%s %x:   %d||Aug 19 22:28:51 hyprframe kernel: ... fixed-purpose events:   4
           1 %D %s %s: %x: %s %x-%s %c.%s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: cfg80211: Loading compiled-in X.509 certificates for regulatory database
           1 %D %s %s: %x: %s %x: %s %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: BNEP filters: protocol multicast
           1 %D %s %s: %x-%s %x-%s: %s %s %s %s %s %s...||Aug 19 23:01:59 hyprframe kernel: cros-ec-dev cros-ec-dev.1.auto: Some logs may have been dropped...
           1 %D %s %s: %x-%s %x-%s: %s %s %s %x-%s %s %s %s -%s||Aug 19 22:28:51 hyprframe kernel: cros-usbpd-charger cros-usbpd-charger.6.auto: probe with driver cros-usbpd-charger failed with error -71
           1 %D %s %s: %x-%s %x-%s: %s %s (%x:%s)||Aug 19 22:28:51 hyprframe kernel: cros-usbpd-charger cros-usbpd-charger.6.auto: Failing probe (err:0xffffffb9)
           1 %D %s %s: %x: %x: %s [%s] (%s %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: battery: Slot [BAT1] (battery present)
           1 %D %s %s: %x-%x-%s %x-%x-%s.%f%x: %s %s %s %s, %s %s||Aug 19 22:28:51 hyprframe kernel: cros-charge-control cros-charge-control.5.auto: Framework charge control detected, preventing load
           2 %D %s %s:     %s||Aug 19 22:28:51 hyprframe kernel:     /init
           2 %D %s %s: %s: %m %s %s (%s=%s)||Aug 20 01:00:10 hyprframe kernel: wlan0: e8:1c:ba:9d:c5:d8 denied association (code=17)
           2 %D %s %s: %s: %s %c.%s %s '%s %s %s %s %s %b'||Aug 19 22:28:51 hyprframe kernel: integrity: Loaded X.509 cert 'Microsoft Windows Production PCA 2011: a92902398e16c49778cd90f99e4f9ae17c55af53'
           2 %D %s %s: %s_%s %d-%d:%f: %s %s = %s||Aug 19 22:28:51 hyprframe kernel: cdc_ncm 2-2:2.0: setting rx_max = 16384
           2 %D %s %s: %s: %s %f %s %s %s, %s %d %s||Aug 19 22:28:51 hyprframe kernel: HugeTLB: registered 1.00 GiB page size, pre-allocated 0 pages
           2 %D %s %s: %s %s:    %s=%d (%s)||Aug 19 22:28:51 hyprframe kernel: snd_hda_codec_idt hdaudioC0D0:    speaker_outs=0 (0x0/0x0/0x0/0x0/0x0)
           2 %D %s %s: %s %s: %s %d [%s %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:16.3: BAR 1 [mem 0x7a411000-0x7a411fff]
           2 %D %s %s: %s %s %s %d [%s  %d-%s %s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: resource 4 [io  0x0000-0x0cf7 window]
           2 %D %s %s: %s %s [%s  %d-%s] %s %s %s||Aug 19 22:28:51 hyprframe kernel: system 00:03: [io  0x1800-0x18fe] has been reserved
           2 %D %s %s: %s: %s %s %s %d %s %s %s||Aug 19 22:28:51 hyprframe kernel: PCI: Using configuration type 1 for base access
           2 %D %s %s: %s: %s %s %s %m (%s=%s %s=%s %s=%d)||Aug 20 01:00:10 hyprframe kernel: wlan0: RX AssocResp from e8:1c:ba:9d:c5:d8 (capab=0x431 status=17 aid=8)
           2 %D %s %s: %s %s %s '%s' %s||Aug 19 22:28:51 hyprframe kernel: Asymmetric key parser 'x509' registered
           2 %D %s %s: %s %s %s %s: %s||Aug 19 22:28:51 hyprframe kernel: Freeing SMP alternatives memory: 52K
           2 %D %s %s: %s %s: %s %s %s %f %s %s||Aug 19 22:28:51 hyprframe kernel: xhci_hcd 0000:00:0d.0: Host supports USB 3.1 Enhanced SuperSpeed
           2 %D %s %s: %s: %s %s%s %s %s||Aug 19 22:28:51 hyprframe kernel: NET: Registered PF_NETLINK/PF_ROUTE protocol family
           2 %D %s %s: %s: %s %s %s '%s_%s'||Aug 19 22:28:51 hyprframe kernel: thermal_sys: Registered thermal governor 'fair_share'
           2 %D %s %s: %s: %s %s %s %s %s %b:%d:%s%b||Aug 19 22:28:51 hyprframe kernel: input: HDA Intel PCH Mic as /devices/pci0000:00/0000:00:1f.3/sound/card0/input14
           2 %D %s %s: %s %s %s %s %s [%s  %d-%s %s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: root bus resource [io  0x0000-0x0cf7 window]
           2 %D %s %s: %s %s: %s %s (%s), %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0: PTM enabled (root), 4ns granularity
           2 %D %s %s: [%s] %s %s %s %s %s %s %d||Aug 19 22:28:51 hyprframe kernel: [drm] Initialized simpledrm 1.0.0 for simple-framebuffer.0 on minor 0
           2 %D %s %s: %s: %s %s %s %s %s %s, %d, %s||Aug 20 01:00:01 hyprframe kernel: wlan0: cannot understand ECSA IE operating class, 4, ignoring
           2 %D %s %s: %s %s %s %s (%s %s) %s: %s||Aug 19 22:28:51 hyprframe kernel: Freeing unused kernel image (text/rodata gap) memory: 764K
           2 %D %s %s: %s: %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: PCI: Using E820 reservations for host bridge windows
           2 %D %s %s: %s %s: [%s] %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:01:00.0: [15b7:5030] type 00 class 0x010802 PCIe Endpoint
           2 %D %s %s: %s_%s_%s %s_%s_%s: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: cros_ec_lpcs cros_ec_lpcs.0: loaded with quirks 00000006
           2 %D %s %s: %s %s: %s,%s: %s %s %s %s [%s %s %d %s] %s %s:%s%s||Aug 19 22:28:51 hyprframe kernel: hid-generic 0003:1050:0407.0001: input,hidraw0: USB HID v1.10 Keyboard [Yubico Yubikey 4 OTP+U2F+CCID] on usb-0000:00:14.0-6/input0
           2 %D %s %s: %s %s: %s: %s %s %s %s %s [%s %s] %s ||Aug 19 22:28:51 hyprframe kernel: hid-generic 001F:8087:0AC2.0005: hidraw4: SENSOR HUB HID v2.00 Device [hid-ishtp 8087:0AC2] on
           2 %D %s %s: %s: %s %s %s %s (%s > %s), %s %s %s %s||Aug 20 11:34:09 hyprframe kernel: perf: interrupt took too long (2503 > 2500), lowering kernel.perf_event_max_sample_rate to 79000
           2 %D %s %s: %s %s: [%s] %s %s %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: [8086:9a49] type 00 class 0x030000 PCIe Root Complex Integrated Endpoint
           2 %D %s %s: %s %s: %s,%s: %s %s %s %s [%s:%s %s] %s %s:%s||Aug 19 22:28:51 hyprframe kernel: hid-generic 0018:093A:0274.0004: input,hidraw3: I2C HID v1.00 Mouse [PIXA3854:00 093A:0274] on i2c-PIXA3854:00
           2 %D %s %s: %s: %s %s %s %s %s %s %s %x %f %s %s||Aug 19 22:28:51 hyprframe kernel: HugeTLB: 16380 KiB vmemmap can be freed for a 1.00 GiB page
           2 %D %s %s: %s %s: [%s] %x: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: simple-framebuffer simple-framebuffer.0: [drm] fb0: simpledrmdrmfb frame buffer device
           2 %D %s %s:   %s %x:||Aug 19 22:28:51 hyprframe kernel:   with arguments:
           2 %D %s %s: %x: %s _%s(%s %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: Added _OSI(Module Device)
           2 %D %s %s: %x: %s: %s %s: %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Firmware SHA1: 0x47cf9d0e
           2 %D %s %s: %x: %s %s_%s_%s=%s, %s_%s=%s||Aug 19 22:28:51 hyprframe kernel: ACPI: EC: EC_CMD/EC_SC=0x66, EC_DATA=0x62
           2 %D %s %s: %x: %s %s %s %s|%s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: DMA: preallocated 4096 KiB GFP_KERNEL|GFP_DMA pool for atomic allocations
           2 %D %s %s: %x-%s %x-%s: %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: cros-usbpd-charger cros-usbpd-charger.6.auto: No USB PD charging ports found
           2 %D %s %s: %x: %x: %s %s [%s]||Aug 19 22:28:51 hyprframe kernel: ACPI: button: Lid Switch [LID0]
           3 %D %s %s: %s %d-%d:%f: %d %s %s||Aug 19 22:28:51 hyprframe kernel: hub 1-0:1.0: 1 port detected
           3 %D %s %s: %s %d-%d: %s %s %s %s, %s=%s, %s=%s, %s= %f||Aug 19 22:28:51 hyprframe kernel: usb 3-6: New USB device found, idVendor=1050, idProduct=0407, bcdDevice= 4.37
           3 %D %s %s: ... %s %s:             %b||Aug 19 22:28:51 hyprframe kernel: ... value mask:             0000ffffffffffff
           3 %D %s %s: %s: %s %c.%s %x: %s:%s||Aug 19 22:28:51 hyprframe kernel: integrity: Loading X.509 certificate: UEFI:db
           3 %D %s %s: %s: %s %s||Aug 19 22:28:51 hyprframe kernel: Yama: becoming mindful.
           3 %D %s %s: %s %s: %s %d [%s  %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: BAR 4 [io  0x3000-0x303f]
           3 %D %s %s: %s %s %s %d [%s %d-%s %s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: resource 6 [mem 0x000a0000-0x000bffff window]
           3 %D %s %s: %s %s: %s %d [%s %d-%s %s]: %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:15.0: BAR 0 [mem 0x4017000000-0x4017000fff 64bit]: assigned
           3 %D %s %s: %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: LSM support for eBPF active
           3 %D %s %s: %s: %s %s %s '%s'||Aug 19 22:28:51 hyprframe kernel: thermal_sys: Registered thermal governor 'step_wise'
           3 %D %s %s: %s %s: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:16.0: PME# supported from D3hot
           3 %D %s %s: %s %s %s %s %s [%s %d-%s %s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: root bus resource [mem 0x000a0000-0x000bffff window]
           3 %D %s %s: %s %s: %s %s (%s %s [%s])||Aug 19 22:28:51 hyprframe kernel: mei_hdcp 0000:00:16.0-b638ab7e-94e2-4ea2-a552-d1c54b627f04: bound 0000:00:02.0 (ops i915_hdcp_ops [i915])
           3 %D %s %s: %s: %s %s: %s=%s %s= %s= %s=%i %s=%i %s=%s %s=%s %s=%s %s=%d %s=%s %s %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=172.20.1.116 DST=224.0.0.251 LEN=68 TOS=0x00 PREC=0x00 TTL=1 ID=50045 DF PROTO=UDP SPT=5353 DPT=5353 LEN=48
           3 %D %s %s: %s %s %s %s %s %s %x-%d %s||Aug 19 22:28:51 hyprframe kernel: Monitor-Mwait will be used to enter C-1 state
           3 %D %s %s: %s %s %s %s %x: %s (%s: %d, %s %s, %s)||Aug 19 22:28:51 hyprframe kernel: IP idents hash table entries: 262144 (order: 9, 2097152 bytes, linear)
           3 %D %s %s: %x: %s %s %f||Aug 19 22:28:51 hyprframe kernel: Bluetooth: Core ver 2.22
           3 %D %s %s: %x: %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: \PIN_: New power resource
           4 %D %s %s: %s %d-%d: %s %s %s %s: %s=%d, %s=%d, %s=%d||Aug 19 22:28:51 hyprframe kernel: usb 2-2: New USB device strings: Mfr=1, Product=2, SerialNumber=7
           4 %D %s %s: %s %s %s %d [%s  %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:02: resource 0 [io  0x4000-0x4fff]
           4 %D %s %s: %s %s %s %d [%s %d-%s %s %s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:02: resource 2 [mem 0x6000000000-0x601bffffff 64bit pref]
           4 %D %s %s: %s %s [%s %d-%s] %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: system 00:02: [mem 0xfed20000-0xfed7ffff] could not be reserved
           4 %D %s %s: %s %s: %s: %s #%d %x- %s %s %x- %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pcieport 0000:00:07.0: pciehp: Slot #0 AttnBtn- PwrCtrl- MRL- AttnInd- PwrInd- HotPlug+ Surprise+ Interlock- NoCompl+ IbPresDis- LLActRep+
           4 %D %s %s: %s: %s %s %s: %s||Aug 19 22:28:51 hyprframe kernel: signal: max sigframe size: 3632
           4 %D %s %s: %s %s:   %s %s [%s  %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0:   bridge window [io  0x4000-0x4fff]
           4 %D %s %s: %s %s: %s %s [%s  %d-%s]: %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0: bridge window [io  0x4000-0x4fff]: assigned
           4 %D %s %s: %s %s: %s %s [%s  %d-%s] %s [%s %s] %s_%s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0: bridge window [io  0x1000-0x0fff] to [bus 02-2b] add_size 1000
           4 %D %s %s: %s: %s:%s %s %s %s %b:%d:%s%s%s%s:%d:%s%s||Aug 19 22:28:51 hyprframe kernel: input: PIXA3854:00 093A:0274 Mouse as /devices/pci0000:00/0000:00:15.3/i2c_designware.2/i2c-16/i2c-PIXA3854:00/0018:093A:0274.0004/input/input10
           4 %D %s %s: %s: %s %s %s %s,%s=%d %s %b:%d:%s%b||Aug 19 22:28:51 hyprframe kernel: input: HDA Intel PCH HDMI/DP,pcm=3 as /devices/pci0000:00/0000:00:1f.3/sound/card0/input16
           4 %D %s %s: %s: %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: raid6: skipped pq benchmark and selected avx512x4
           4 %D %s %s: %s: %s %s %s %s %s_%s||Aug 19 22:28:51 hyprframe kernel: usbcore: registered new interface driver cdc_ether
           4 %D %s %s: %s %s: %s %s %s %s: %s=%d, %s=%d, %s=%d||Aug 19 22:28:51 hyprframe kernel: usb usb1: New USB device strings: Mfr=3, Product=2, SerialNumber=1
           4 %D %s %s: %s %s: %s %s %s %s %s %d^-%s %s||Aug 19 22:28:51 hyprframe kernel: RAPL PMU: hw unit of domain pp0-core 2^-14 Joules
           4 %D %s %s: %s %s: %s %s %s %s %s %s %d||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0: Overriding RP PIO Log Size to 4
           4 %D %s %s: %s %s: %s %s %s %s, %s %s %s %d||Aug 19 22:28:51 hyprframe kernel: xhci_hcd 0000:00:0d.0: new USB bus registered, assigned bus number 1
           4 %D %s %s: %s %s: %s %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:0d.2: PME# supported from D0 D1 D2 D3hot D3cold
           4 %D %s %s: %s %s: %s %s %s %s, %s=%s, %s=%s, %s= %f||Aug 19 22:28:51 hyprframe kernel: usb usb1: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 6.15
           4 %D %s %s: %x: %s %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: DMA: preallocated 4096 KiB GFP_KERNEL pool for atomic allocations
           4 %D %s %s: %x: %s %s %s (%s %s  %s  %s %s %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: SSDT 0xFFFF8CBC4222D000 000386 (v02 PmRef  Cpu0Cst  00003001 INTL 20160422)
           4 %D %s %s: %x: %s %s %s (%s %s  %s    %s %s %s)||Aug 19 22:28:51 hyprframe kernel: ACPI: SSDT 0xFFFF8CBC42237000 0008E7 (v02 PmRef  ApIst    00003000 INTL 20160422)
           5 %D %s %s: %s %d-%d:%f: %s %s %s||Aug 19 22:28:51 hyprframe kernel: hub 1-0:1.0: USB hub found
           5 %D %s %s: %s %d-%d: %s: %s||Aug 19 22:28:51 hyprframe kernel: usb 2-2: Manufacturer: Realtek
           5 %D %s %s: %s: %s %s %s %m %s||Aug 20 00:56:45 hyprframe kernel: wlan0: Connection to AP e8:1c:ba:9d:c5:e0 lost
           5 %D %s %s: %s %s: %s_%s: %s||Aug 19 22:28:51 hyprframe kernel: iwlwifi 0000:aa:00.0: CNVI_SCU_SEQ_DATA_DW9: 0x0
           6 %D %s %s: %s %s %s %d [%s %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:01: resource 1 [mem 0x7a300000-0x7a3fffff]
           6 %D %s %s: %s %s: %s %s (%s -> %s)||Aug 19 22:28:51 hyprframe kernel: intel-lpss 0000:00:15.0: enabling device (0004 -> 0006)
           6 %D %s %s: %s %s: %s: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pcieport 0000:00:06.0: PME: Signaling with IRQ 126
           6 %D %s %s: %x: %x: %s %s %s||Aug 19 22:28:51 hyprframe kernel: DMAR: dmar4: Using Queued invalidation
           7 %D %s %s: %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: landlock: Up and running.
           7 %D %s %s: %s %s: %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0: PME# supported from D0 D3hot D3cold
           7 %D %s %s: %x: %s %s %s [%s %d-%s]||Aug 19 22:28:51 hyprframe kernel: e820: reserve RAM buffer [mem 0x0009f000-0x0009ffff]
           8 %D %s %s: %s %s:   %s %s [%s %d-%s %s %s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0:   bridge window [mem 0x6000000000-0x601bffffff 64bit pref]
           8 %D %s %s: %s %s %s %x: %s (%s: %d, %s %s, %s)||Aug 19 22:28:51 hyprframe kernel: Mount-cache hash table entries: 131072 (order: 8, 1048576 bytes, linear)
           8 %D %s %s: %x: %s %s %s %s:||Aug 19 22:28:51 hyprframe kernel: ACPI: Dynamic OEM Table Load:
          10 %D %s %s: %s %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:0a.0: enabling Extended Tags
          10 %D %s %s: %s %s: %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: vgaarb: bridge control possible
          10 %D %s %s: %s: %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: usbcore: registered new interface driver usbfs
          10 %D %s %s: %s %s: %s %s %s %s %d||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.3: Adding to iommu group 0
          12 %D %s %s: %s %s [%s %d-%s] %s %s %s||Aug 19 22:28:51 hyprframe kernel: system 00:02: [mem 0xfedc0000-0xfedc7fff] has been reserved
          12 %D %s %s: %s %s:   %s %s [%s %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0:   bridge window [mem 0x7a300000-0x7a3fffff]
          12 %D %s %s: %x: %s %s %s||Aug 19 22:28:51 hyprframe kernel: cpuidle: using governor ladder
          12 %D %s %s: %x: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: bus type USB registered
          14 %D %s %s: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: Initialise system trusted keyrings
          17 %D %s %s: %s: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: raid6: using avx512x2 recovery algorithm
          18 %D %s %s: %s: %s %s %m (%s: %d=<%s>)||Aug 20 00:22:05 hyprframe kernel: wlan0: deauthenticated from e8:1c:ba:9d:c5:e0 (Reason: 0=<unknown>)
          18 %D %s %s: %s %s: %s %s %s [%s %s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0: PCI bridge to [bus 01]
          19 %D %s %s: %s %s: %s: %s||Aug 19 22:28:51 hyprframe kernel: usb usb1: SerialNumber: 0000:00:0d.0
          19 %D %s %s: %x: %c_%s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: \_SB_.PC00.PEG0.PXP_: New power resource
          22 %D %s %s: %s %s: %s %d [%s %d-%s %s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: BAR 0 [mem 0x605c000000-0x605cffffff 64bit]
          24 %D %s %s: %s %s: [%s] %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:00.0: [8086:9a14] type 00 class 0x060000 conventional PCI endpoint
          28 %D %s %s: %s %s: %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:0d.0: PME# supported from D3hot D3cold
          28 %D %s %s: %s: %s %s: %s=%s %s= %s=%m:%m:%s %s=%x:%s %s=%x:%s %s=%s %s=%d %s=%s %s=%d %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:a0:d3:c1:83:e3:a8:86:dd SRC=fe80:0000:0000:0000:a2d3:c1ff:fe83:e3a8 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=896 TC=0 HOPLIMIT=255 FLOWLBL=0 PROTO=UDP SPT=5353 DPT=5353 LEN=856
          31 %D %s %s: %s: %s %s: %s=%s %s= %s= %s=%x:%s %s=%x:%s %s=%s %s=%d %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=fe80:0000:0000:0000:90d3:2ee5:3d79:3b30 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=189 TC=0 HOPLIMIT=255 FLOWLBL=26027 PROTO=UDP SPT=5353 DPT=5353 LEN=149
          32 %D %s %s: %s %d-%d: %s %x-%s %s %s %s %d %s %s||Aug 19 22:28:51 hyprframe kernel: usb 3-6: new full-speed USB device number 2 using xhci_hcd
          47 %D %s %s: %s: %s %s %m %s %s||Aug 19 23:06:46 hyprframe kernel: wlan0: association with e8:1c:ba:9d:c5:e0 timed out
          57 %D %s %s: %s: %s %s %m (%s: %d=%s_%s)||Aug 20 01:35:58 hyprframe kernel: wlan0: disassociated from e8:1c:ba:73:3c:00 (Reason: 8=DISASSOC_STA_HAS_LEFT)
          70 %D %s %s: %s: %s %s: %s=%s %s= %s= %s=%i %s=%i %s=%s %s=%s %s=%s %s=%s %s=%s %s %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=172.20.1.116 DST=224.0.0.251 LEN=238 TOS=0x00 PREC=0x00 TTL=255 ID=46274 DF PROTO=UDP SPT=5353 DPT=5353 LEN=218
          82 %D %s %s: %s: %s %s %s %m %s %s %s %s %m||Aug 19 22:28:51 hyprframe kernel: wlan0: disconnect from AP e8:1c:ba:9d:c5:e0 for new auth to e8:1c:ba:73:3c:00
         126 %D %s %s: %s: %s %s: %s=%s %s= %s=%m:%m:%s %s=%x:%s %s=%x:%s %s=%s %s=%d %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:f4:dd:06:3a:3e:b9:86:dd SRC=fe80:0000:0000:0000:f6dd:06ff:fe3a:3eb9 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=208 TC=0 HOPLIMIT=255 FLOWLBL=1032098 PROTO=UDP SPT=5353 DPT=5353 LEN=168
         156 %D %s %s: %s: %b %s %m %s %s %s (%s: %d=%s_%s)||Aug 19 22:32:22 hyprframe kernel: wlan0: deauthenticating from e8:1c:ba:73:3c:00 by local choice (Reason: 3=DEAUTH_LEAVING)
         308 %D %s %s: %s: %s %s %s %s %s (%s - %d) %s %s %s %s %m||Aug 19 22:28:51 hyprframe kernel: wlan0: Limiting TX power to 21 (24 - 3) dBm as advertised by e8:1c:ba:9d:c5:e0
         317 %D %s %s: %s: %s %s %s %m (%s=%s %s=%d %s=%d)||Aug 19 22:28:51 hyprframe kernel: wlan0: RX AssocResp from e8:1c:ba:9d:c5:e0 (capab=0x511 status=0 aid=3)
         367 %D %s %s: %s: %s %s %m (%s %s=%m)||Aug 19 22:28:51 hyprframe kernel: wlan0: authenticate with e8:1c:ba:9d:c5:e0 (local address=96:11:9b:a3:51:45)
         370 %D %s %s: %s: %s %s %s %m (%s %s)||Aug 19 22:28:51 hyprframe kernel: wlan0: send auth to e8:1c:ba:9d:c5:e0 (try 1/3)
         501 %D %s %s: %s: %s %s %m (%s %s)||Aug 19 22:28:51 hyprframe kernel: wlan0: associate with e8:1c:ba:9d:c5:e0 (try 1/3)
         530 %D %s %s: %s: %s %s: %s=%s %s= %s=%m:%m:%s %s=%i %s=%i %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=01:00:5e:00:00:fb:a0:d3:c1:83:e3:a8:08:00 SRC=172.20.1.113 DST=224.0.0.251 LEN=861 TOS=0x00 PREC=0x00 TTL=255 ID=1497 PROTO=UDP SPT=5353 DPT=5353 LEN=841
         686 %D %s %s: %s: %s||Aug 19 22:28:51 hyprframe kernel: NetLabel: Initializing
        1179 %D %s %s: %s: %s %s: %s=%s %s= %s=%m:%m:%s %s=%x:%s %s=%x:%s %s=%s %s=%d %s=%d %s=%s %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:74:86:e2:26:ef:c8:86:dd SRC=fe80:0000:0000:0000:2eb1:a4f9:99ac:2395 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=92 TC=0 HOPLIMIT=1 FLOWLBL=248228 PROTO=UDP SPT=5353 DPT=5353 LEN=52
        1188 %D %s %s: %s: %s %s: %s=%s %s= %s=%m:%m:%s %s=%i %s=%i %s=%s %s=%s %s=%s %s=%d %s=%s %s=%s %s=%s %s=%s %s=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=01:00:5e:00:00:fb:74:86:e2:26:ef:c8:08:00 SRC=172.20.1.110 DST=224.0.0.251 LEN=72 TOS=0x00 PREC=0x00 TTL=1 ID=62314 PROTO=UDP SPT=5353 DPT=5353 LEN=52
```

### BASE64 Field Detection

Tmpltr automatically detects and templates BASE64-encoded data, preventing template explosion from unique encoded payloads:

```
$ echo "PacketData:3KYyaJoq1Haglac3CABFAAAo1B5AACgGqh3BLv8zCgoKKNrCIPsgujelNIOzJVAQchAtagAAAAAAAAAADg==" | ./tmpltr -
           1 %s:%b||PacketData:3KYyaJoq1Haglac3CABFAAAo1B5AACgGqh3BLv8zCgoKKNrCIPsgujelNIOzJVAQchAtagAAAAAAAAAADg==
```

This prevents millions of unique BASE64 strings from creating separate templates, instead recognizing the pattern `%s:%b` for all `FieldName:BASE64Data` combinations.

If the standard templating mode is obscuring too much information, you can switch to clustering mode (-c).  This reduces the parsing speed, but allows the templates to retain all of the non-variable strings.  I normally strip off the trailing example line when running in this mode just to keep the line length more manageable.  Below is an example of the same log processed with the clustering (-c) parsing option.

```
% tmpltr -c /var/log/syslog | sort -n
           1 %D hyprframe kernel: rcu: Max phase no-delay instances is 400.
           1 %D hyprframe kernel: registered taskstats version 1
           1 %D hyprframe kernel: resource: avoiding allocation from e820 entry [mem 0x0009f000-0x000fffff]
           1 %D hyprframe kernel: resource: resource sanity check: requesting [mem 0x00000000fedc0000-0x00000000fedcdfff], which spans more than pnp 00:02 [mem 0xfedc0000-0xfedc7fff]
           1 %D hyprframe kernel: rtc_cmos rtc_cmos: alarms up to one month, y3k, 114 bytes nvram
           1 %D hyprframe kernel: rtc_cmos rtc_cmos: setting system clock to 2025-08-20T05:25:11 UTC (1755667511)
           1 %D hyprframe kernel: sched_clock: Marking stable (1133001327, 21021065)->(1184912321, -30889929)
           1 %D hyprframe kernel: SCSI subsystem initialized
           1 %D hyprframe kernel: Serial: 8250/16550 driver, 32 ports, IRQ sharing enabled
           1 %D hyprframe kernel: serio: i8042 AUX port at 0x60,0x64 irq 12
           1 %D hyprframe kernel: serio: i8042 KBD port at 0x60,0x64 irq 1
           1 %D hyprframe kernel: simple-framebuffer simple-framebuffer.0: [drm] Registered 1 planes with drm panic
           1 %D hyprframe kernel: smpboot: CPU0: 11th Gen Intel(R) Core(TM) i7-1185G7 @ 3.00GHz (family: 0x6, model: 0x8c, stepping: 0x1)
           1 %D hyprframe kernel: smpboot: Total of 8 processors activated (47923.20 BogoMIPS)
           1 %D hyprframe kernel: smpboot: x86: Booting SMP configuration:
           1 %D hyprframe kernel: smp: Bringing up secondary CPUs ...
           1 %D hyprframe kernel: smp: Brought up 1 node, 8 CPUs
           1 %D hyprframe kernel: snd_hda_codec_idt hdaudioC0D0: autoconfig for 92HD95: line_outs=1 (0xd/0x0/0x0/0x0/0x0) type:speaker
           1 %D hyprframe kernel: snd_hda_codec_idt hdaudioC0D0:    inputs:
           1 %D hyprframe kernel: snd_hda_codec_idt hdaudioC0D0:      Internal Mic=0xe
           1 %D hyprframe kernel: snd_hda_codec_idt hdaudioC0D0:      Mic=0xb
           1 %D hyprframe kernel: snd_hda_codec_idt hdaudioC0D0:    mono: mono_out=0x0
           1 %D hyprframe kernel: software IO TLB: mapped [mem 0x000000003af97000-0x000000003ef97000] (64MB)
           1 %D hyprframe kernel: spi-nor spi0.0: supply vcc not found, using dummy regulator
           1 %D hyprframe kernel: TCP established hash table entries: 524288 (order: 10, 4194304 bytes, linear)
           1 %D hyprframe kernel: TCP: Hash tables configured (established 524288 bind 65536)
           1 %D hyprframe kernel:     TERM=linux
           1 %D hyprframe kernel: thermal LNXTHERM:00: registered as thermal_zone0
           1 %D hyprframe kernel: Timer migration: 1 hierarchy levels; 8 children per group; 1 crossnode level
           1 %D hyprframe kernel: tpm_tis NTC0702:00: 2.0 TPM (device-id 0xFC, rev-id 1)
           1 %D hyprframe kernel: Trying to unpack rootfs image as initramfs...
           1 %D hyprframe kernel: usb 2-2: new SuperSpeed Plus Gen 2x1 USB device number 2 using xhci_hcd
           1 %D hyprframe kernel: usb 2-2: New USB device found, idVendor=0bda, idProduct=8157, bcdDevice=30.00
           1 %D hyprframe kernel: usb 2-2: Product: USB 10/100/1G/2.5G/5G LAN
           1 %D hyprframe kernel: usb 2-2: SerialNumber: 0003002427890BAB
           1 %D hyprframe kernel: usb 3-10: new full-speed USB device number 5 using xhci_hcd
           1 %D hyprframe kernel: usb 3-10: New USB device found, idVendor=8087, idProduct=0032, bcdDevice= 0.00
           1 %D hyprframe kernel: usb 3-10: New USB device strings: Mfr=0, Product=0, SerialNumber=0
           1 %D hyprframe kernel: usb 3-6: Product: Yubikey 4 OTP+U2F+CCID
           1 %D hyprframe kernel: usb 3-7: Found UVC 1.00 device Laptop Camera (0bda:5634)
           1 %D hyprframe kernel: usb 3-7: new high-speed USB device number 3 using xhci_hcd
           1 %D hyprframe kernel: usb 3-7: Product: Laptop Camera
           1 %D hyprframe kernel: usb 3-9: Manufacturer: Goodix Technology Co., Ltd.
           1 %D hyprframe kernel: usb 3-9: Product: Goodix Fingerprint USB Device
           1 %D hyprframe kernel: ... version:                5
           1 %D hyprframe kernel: VFS: Disk quotas dquot_
           1 %D hyprframe kernel: VFS: Dquot-cache hash table entries: 512 (order 0, 4096 bytes)
           1 %D hyprframe kernel: videodev: Linux video capture interface: v2.00
           1 %D hyprframe kernel: warning: `ThreadPoolForeg' uses wireless extensions which will stop working for Wi-Fi 7 hardware; use nl80211
           1 %D hyprframe kernel: wlan0: deauthenticated from e8:1c:ba:9d:c5:e0 while associating (Reason: 0=<unknown>)
           1 %D hyprframe kernel: workingset: timestamp_bits=36 max_order=24 bucket_order=0
           1 %D hyprframe kernel: Write protecting the kernel read-only data: 38912k
           1 %D hyprframe kernel: x86/fpu: Enabled xstate features 0xae7, context size is 2456 bytes, using 'compacted' format.
           1 %D hyprframe kernel: x86/fpu: xstate_offset[11]: 2440, xstate_sizes[11]:   16
           1 %D hyprframe kernel: x86/fpu: xstate_offset[7]: 1408, xstate_sizes[7]: 1024
           1 %D hyprframe kernel: x86/fpu: xstate_offset[9]: 2432, xstate_sizes[9]:    8
           1 %D hyprframe kernel: x86/mm: Checked W+X mappings: passed, no W+X pages found.
           1 %D hyprframe kernel: xor: automatically using best checksumming function   avx
           1 %D hyprframe kernel: zram0: detected capacity change from 0 to 8388608
           1 %D hyprframe kernel: zram: Added device: zram0
           2 %D hyprframe kernel: ACPI: Added _OSI(%s Device)||Aug 19 22:28:51 hyprframe kernel: ACPI: Added _OSI(Module Device)
           2 %D hyprframe kernel: ACPI: button: %s %s [%s]||Aug 19 22:28:51 hyprframe kernel: ACPI: button: Lid Switch [LID0]
           2 %D hyprframe kernel: ACPI: EC: EC_CMD/EC_SC=0x66, EC_DATA=0x62||Aug 19 22:28:51 hyprframe kernel: ACPI: EC: EC_CMD/EC_SC=0x66, EC_DATA=0x62
           2 %D hyprframe kernel: Asymmetric key parser '%s' registered||Aug 19 22:28:51 hyprframe kernel: Asymmetric key parser 'x509' registered
           2 %D hyprframe kernel: Bluetooth: hci0: Firmware %s: %s||Aug 19 22:28:51 hyprframe kernel: Bluetooth: hci0: Firmware SHA1: 0x47cf9d0e
           2 %D hyprframe kernel: cdc_ncm 2-2:2.0: setting %s = 16384||Aug 19 22:28:51 hyprframe kernel: cdc_ncm 2-2:2.0: setting rx_max = 16384
           2 %D hyprframe kernel: cros_ec_lpcs cros_ec_lpcs.0: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: cros_ec_lpcs cros_ec_lpcs.0: loaded with quirks 00000006
           2 %D hyprframe kernel: cros-usbpd-charger cros-usbpd-charger.6.auto: %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: cros-usbpd-charger cros-usbpd-charger.6.auto: No USB PD charging ports found
           2 %D hyprframe kernel: DMA: preallocated 4096 KiB GFP_KERNEL|%s pool for atomic allocations||Aug 19 22:28:51 hyprframe kernel: DMA: preallocated 4096 KiB GFP_KERNEL|GFP_DMA pool for atomic allocations
           2 %D hyprframe kernel: [drm] Initialized %s %s %s on minor %d||Aug 19 22:28:51 hyprframe kernel: [drm] Initialized simpledrm 1.0.0 for simple-framebuffer.0 on minor 0
           2 %D hyprframe kernel: Freeing %s %s memory: %s||Aug 19 22:28:51 hyprframe kernel: Freeing SMP alternatives memory: 52K
           2 %D hyprframe kernel: Freeing unused kernel image (%s gap) memory: %s||Aug 19 22:28:51 hyprframe kernel: Freeing unused kernel image (text/rodata gap) memory: 764K
           2 %D hyprframe kernel: hid-generic %s: %s,%s: USB HID v1.10 %s [Yubico Yubikey 4 OTP+U2F+CCID] on usb-0000:00:14.0-6%s||Aug 19 22:28:51 hyprframe kernel: hid-generic 0003:1050:0407.0001: input,hidraw0: USB HID v1.10 Keyboard [Yubico Yubikey 4 OTP+U2F+CCID] on usb-0000:00:14.0-6/input0
           2 %D hyprframe kernel: HugeTLB: registered %f %s page size, pre-allocated 0 pages||Aug 19 22:28:51 hyprframe kernel: HugeTLB: registered 1.00 GiB page size, pre-allocated 0 pages
           2 %D hyprframe kernel: HugeTLB: %s KiB vmemmap can be freed for a %f %s page||Aug 19 22:28:51 hyprframe kernel: HugeTLB: 16380 KiB vmemmap can be freed for a 1.00 GiB page
           2 %D hyprframe kernel: input: HDA Intel PCH %s as /devices/pci0000:00/0000:00:1f.3%b||Aug 19 22:28:51 hyprframe kernel: input: HDA Intel PCH Mic as /devices/pci0000:00/0000:00:1f.3/sound/card0/input14
           2 %D hyprframe kernel: integrity: Loaded X.509 cert 'Microsoft %s %s %s 2011: %b'||Aug 19 22:28:51 hyprframe kernel: integrity: Loaded X.509 cert 'Microsoft Windows Production PCA 2011: a92902398e16c49778cd90f99e4f9ae17c55af53'
           2 %D hyprframe kernel: NET: Registered %s%s protocol family||Aug 19 22:28:51 hyprframe kernel: NET: Registered PF_NETLINK/PF_ROUTE protocol family
           2 %D hyprframe kernel: pci_bus 0000:00: resource %d [io  %d-%s window]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: resource 4 [io  0x0000-0x0cf7 window]
           2 %D hyprframe kernel: pci_bus 0000:00: root bus resource [io  %d-%s window]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: root bus resource [io  0x0000-0x0cf7 window]
           2 %D hyprframe kernel: pci %s: BAR %d [mem %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:16.3: BAR 1 [mem 0x7a411000-0x7a411fff]
           2 %D hyprframe kernel: pci %s: PTM enabled (root), 4ns granularity||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0: PTM enabled (root), 4ns granularity
           2 %D hyprframe kernel: pci %s: [%s] type 00 class %s PCIe Endpoint||Aug 19 22:28:51 hyprframe kernel: pci 0000:01:00.0: [15b7:5030] type 00 class 0x010802 PCIe Endpoint
           2 %D hyprframe kernel: pci %s: [%s] type 00 class %s PCIe Root Complex Integrated Endpoint||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: [8086:9a49] type 00 class 0x030000 PCIe Root Complex Integrated Endpoint
           2 %D hyprframe kernel: perf: interrupt took too long (%s > %s), lowering kernel.perf_event_max_sample_rate to %s||Aug 20 11:34:09 hyprframe kernel: perf: interrupt took too long (2503 > 2500), lowering kernel.perf_event_max_sample_rate to 79000
           2 %D hyprframe kernel: %s 0018:093A:0274.0004: input,hidraw3: I2C HID v1.00 Mouse [PIXA3854:00 093A:0274] on i2c-PIXA3854:00||Aug 19 22:28:51 hyprframe kernel: hid-generic 0018:093A:0274.0004: input,hidraw3: I2C HID v1.00 Mouse [PIXA3854:00 093A:0274] on i2c-PIXA3854:00
           2 %D hyprframe kernel: %s 001F:8087:0AC2.0005: hidraw4: SENSOR HUB HID v2.00 Device [hid-ishtp 8087:0AC2] on ||Aug 19 22:28:51 hyprframe kernel: hid-generic 001F:8087:0AC2.0005: hidraw4: SENSOR HUB HID v2.00 Device [hid-ishtp 8087:0AC2] on
           2 %D hyprframe kernel:     %s||Aug 19 22:28:51 hyprframe kernel:     /init
           2 %D hyprframe kernel: snd_hda_codec_idt hdaudioC0D0:    %s=%d (%s)||Aug 19 22:28:51 hyprframe kernel: snd_hda_codec_idt hdaudioC0D0:    speaker_outs=0 (0x0/0x0/0x0/0x0/0x0)
           2 %D hyprframe kernel: %s %s: [drm] fb0: %s frame buffer device||Aug 19 22:28:51 hyprframe kernel: simple-framebuffer simple-framebuffer.0: [drm] fb0: simpledrmdrmfb frame buffer device
           2 %D hyprframe kernel: %s: %s %s %s %d %s %s %s||Aug 19 22:28:51 hyprframe kernel: PCI: Using configuration type 1 for base access
           2 %D hyprframe kernel: %s: %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: PCI: Using E820 reservations for host bridge windows
           2 %D hyprframe kernel: system %s [io  %d-%s] has been reserved||Aug 19 22:28:51 hyprframe kernel: system 00:03: [io  0x1800-0x18fe] has been reserved
           2 %D hyprframe kernel: thermal_sys: Registered thermal governor '%s_%s'||Aug 19 22:28:51 hyprframe kernel: thermal_sys: Registered thermal governor 'fair_share'
           2 %D hyprframe kernel:   with %x:||Aug 19 22:28:51 hyprframe kernel:   with arguments:
           2 %D hyprframe kernel: wlan0: cannot understand ECSA IE operating class, 4, ignoring||Aug 20 01:00:01 hyprframe kernel: wlan0: cannot understand ECSA IE operating class, 4, ignoring
           2 %D hyprframe kernel: wlan0: e8:1c:ba:9d:c5:d8 denied association (code=17)||Aug 20 01:00:10 hyprframe kernel: wlan0: e8:1c:ba:9d:c5:d8 denied association (code=17)
           2 %D hyprframe kernel: wlan0: RX AssocResp from e8:1c:ba:9d:c5:d8 (capab=0x431 status=17 aid=8)||Aug 20 01:00:10 hyprframe kernel: wlan0: RX AssocResp from e8:1c:ba:9d:c5:d8 (capab=0x431 status=17 aid=8)
           2 %D hyprframe kernel: xhci_hcd %s: Host supports USB 3.1 Enhanced SuperSpeed||Aug 19 22:28:51 hyprframe kernel: xhci_hcd 0000:00:0d.0: Host supports USB 3.1 Enhanced SuperSpeed
           3 %D hyprframe kernel: Bluetooth: %s ver %f||Aug 19 22:28:51 hyprframe kernel: Bluetooth: Core ver 2.22
           3 %D hyprframe kernel: hub %d-0:1.0: %d %s detected||Aug 19 22:28:51 hyprframe kernel: hub 1-0:1.0: 1 port detected
           3 %D hyprframe kernel: integrity: Loading X.509 certificate: UEFI:db||Aug 19 22:28:51 hyprframe kernel: integrity: Loading X.509 certificate: UEFI:db
           3 %D hyprframe kernel: Monitor-Mwait will be used to enter C-%d state||Aug 19 22:28:51 hyprframe kernel: Monitor-Mwait will be used to enter C-1 state
           3 %D hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=172.20.1.116 DST=224.0.0.251 LEN=68 TOS=0x00 PREC=0x00 TTL=1 ID=%s DF PROTO=UDP SPT=5353 DPT=5353 LEN=48 ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=172.20.1.116 DST=224.0.0.251 LEN=68 TOS=0x00 PREC=0x00 TTL=1 ID=50045 DF PROTO=UDP SPT=5353 DPT=5353 LEN=48
           3 %D hyprframe kernel: pci_bus 0000:00: resource %d [mem %d-%s window]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: resource 6 [mem 0x000a0000-0x000bffff window]
           3 %D hyprframe kernel: pci_bus 0000:00: root bus resource [mem %d-%s window]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:00: root bus resource [mem 0x000a0000-0x000bffff window]
           3 %D hyprframe kernel: pci %s: BAR 0 [mem %d-%s 64bit]: assigned||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:15.0: BAR 0 [mem 0x4017000000-0x4017000fff 64bit]: assigned
           3 %D hyprframe kernel: pci %s: BAR %d [io  %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: BAR 4 [io  0x3000-0x303f]
           3 %D hyprframe kernel: ... %s %s:             %b||Aug 19 22:28:51 hyprframe kernel: ... value mask:             0000ffffffffffff
           3 %D hyprframe kernel: %s %s: bound 0000:00:02.0 (ops %s [i915])||Aug 19 22:28:51 hyprframe kernel: mei_hdcp 0000:00:16.0-b638ab7e-94e2-4ea2-a552-d1c54b627f04: bound 0000:00:02.0 (ops i915_hdcp_ops [i915])
           3 %D hyprframe kernel: %s %s hash table entries: %s (order: 9, 2097152 bytes, linear)||Aug 19 22:28:51 hyprframe kernel: IP idents hash table entries: 262144 (order: 9, 2097152 bytes, linear)
           3 %D hyprframe kernel: %s: %s %s||Aug 19 22:28:51 hyprframe kernel: Yama: becoming mindful.
           3 %D hyprframe kernel: %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: LSM support for eBPF active
           3 %D hyprframe kernel: %s %s: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:16.0: PME# supported from D3hot
           3 %D hyprframe kernel: thermal_sys: Registered thermal governor '%s'||Aug 19 22:28:51 hyprframe kernel: thermal_sys: Registered thermal governor 'step_wise'
           3 %D hyprframe kernel: usb 3-%d: New USB device found, idVendor=%s, idProduct=%s, bcdDevice= %f||Aug 19 22:28:51 hyprframe kernel: usb 3-6: New USB device found, idVendor=1050, idProduct=0407, bcdDevice= 4.37
           3 %D hyprframe kernel: %x: %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: \PIN_: New power resource
           4 %D hyprframe kernel: ACPI: SSDT %s %s (v02 PmRef  %s    00003000 INTL 20160422)||Aug 19 22:28:51 hyprframe kernel: ACPI: SSDT 0xFFFF8CBC42237000 0008E7 (v02 PmRef  ApIst    00003000 INTL 20160422)
           4 %D hyprframe kernel: ACPI: SSDT %s %s (v02 PmRef  %s  %s INTL 20160422)||Aug 19 22:28:51 hyprframe kernel: ACPI: SSDT 0xFFFF8CBC4222D000 000386 (v02 PmRef  Cpu0Cst  00003001 INTL 20160422)
           4 %D hyprframe kernel: input: HDA Intel PCH HDMI/DP,pcm=%d as /devices/pci0000:00/0000:00:1f.3%b||Aug 19 22:28:51 hyprframe kernel: input: HDA Intel PCH HDMI/DP,pcm=3 as /devices/pci0000:00/0000:00:1f.3/sound/card0/input16
           4 %D hyprframe kernel: input: PIXA3854:00 093A:0274 %s as /devices/pci0000:00/0000:00:15.3/i2c_designware.2/i2c-16/i2c-PIXA3854:00/0018:093A:0274.0004%s||Aug 19 22:28:51 hyprframe kernel: input: PIXA3854:00 093A:0274 Mouse as /devices/pci0000:00/0000:00:15.3/i2c_designware.2/i2c-16/i2c-PIXA3854:00/0018:093A:0274.0004/input/input10
           4 %D hyprframe kernel: pci_bus %s resource 0 [io  %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:02: resource 0 [io  0x4000-0x4fff]
           4 %D hyprframe kernel: pci_bus %s resource 2 [mem %d-%s 64bit pref]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:02: resource 2 [mem 0x6000000000-0x601bffffff 64bit pref]
           4 %D hyprframe kernel: pcieport %s: pciehp: Slot #0 AttnBtn- PwrCtrl- MRL- AttnInd- PwrInd- HotPlug+ Surprise+ Interlock- NoCompl+ IbPresDis- LLActRep+||Aug 19 22:28:51 hyprframe kernel: pcieport 0000:00:07.0: pciehp: Slot #0 AttnBtn- PwrCtrl- MRL- AttnInd- PwrInd- HotPlug+ Surprise+ Interlock- NoCompl+ IbPresDis- LLActRep+
           4 %D hyprframe kernel: pci %s: bridge window [io  0x1000-0x0fff] to [bus %s] add_size 1000||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0: bridge window [io  0x1000-0x0fff] to [bus 02-2b] add_size 1000
           4 %D hyprframe kernel: pci %s: bridge window [io  %d-%s]: assigned||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0: bridge window [io  0x4000-0x4fff]: assigned
           4 %D hyprframe kernel: pci %s:   bridge window [io  %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0:   bridge window [io  0x4000-0x4fff]
           4 %D hyprframe kernel: pci %s: Overriding RP PIO Log Size to 4||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0: Overriding RP PIO Log Size to 4
           4 %D hyprframe kernel: pci %s: PME# supported from D0 D1 D2 D3hot D3cold||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:0d.2: PME# supported from D0 D1 D2 D3hot D3cold
           4 %D hyprframe kernel: RAPL PMU: hw unit of domain %s 2^-14 Joules||Aug 19 22:28:51 hyprframe kernel: RAPL PMU: hw unit of domain pp0-core 2^-14 Joules
           4 %D hyprframe kernel: %s: %s %s size: %s||Aug 19 22:28:51 hyprframe kernel: signal: max sigframe size: 3632
           4 %D hyprframe kernel: %s: %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: raid6: skipped pq benchmark and selected avx512x4
           4 %D hyprframe kernel: system 00:02: [mem %d-%s] could not be reserved||Aug 19 22:28:51 hyprframe kernel: system 00:02: [mem 0xfed20000-0xfed7ffff] could not be reserved
           4 %D hyprframe kernel: usbcore: registered new interface driver cdc_%s||Aug 19 22:28:51 hyprframe kernel: usbcore: registered new interface driver cdc_ether
           4 %D hyprframe kernel: usb %d-%d: New USB device strings: Mfr=1, Product=2, SerialNumber=%d||Aug 19 22:28:51 hyprframe kernel: usb 2-2: New USB device strings: Mfr=1, Product=2, SerialNumber=7
           4 %D hyprframe kernel: usb %s: New USB device found, idVendor=1d6b, idProduct=%s, bcdDevice= 6.15||Aug 19 22:28:51 hyprframe kernel: usb usb1: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 6.15
           4 %D hyprframe kernel: usb %s: New USB device strings: Mfr=3, Product=2, SerialNumber=1||Aug 19 22:28:51 hyprframe kernel: usb usb1: New USB device strings: Mfr=3, Product=2, SerialNumber=1
           4 %D hyprframe kernel: xhci_hcd 0000:00:0d.0: new USB bus registered, assigned bus number %d||Aug 19 22:28:51 hyprframe kernel: xhci_hcd 0000:00:0d.0: new USB bus registered, assigned bus number 1
           4 %D hyprframe kernel: %x: %s %s %s %s %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: DMA: preallocated 4096 KiB GFP_KERNEL pool for atomic allocations
           5 %D hyprframe kernel: hub %d-0:1.0: USB hub found||Aug 19 22:28:51 hyprframe kernel: hub 1-0:1.0: USB hub found
           5 %D hyprframe kernel: iwlwifi 0000:aa:00.0: CNVI_SCU_SEQ_DATA_DW9: 0x0||Aug 19 22:28:51 hyprframe kernel: iwlwifi 0000:aa:00.0: CNVI_SCU_SEQ_DATA_DW9: 0x0
           5 %D hyprframe kernel: usb %d-%d: Manufacturer: %s||Aug 19 22:28:51 hyprframe kernel: usb 2-2: Manufacturer: Realtek
           5 %D hyprframe kernel: wlan0: Connection to AP %m lost||Aug 20 00:56:45 hyprframe kernel: wlan0: Connection to AP e8:1c:ba:9d:c5:e0 lost
           6 %D hyprframe kernel: DMAR: %x: Using Queued invalidation||Aug 19 22:28:51 hyprframe kernel: DMAR: dmar4: Using Queued invalidation
           6 %D hyprframe kernel: pci_bus %s resource 1 [mem %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci_bus 0000:01: resource 1 [mem 0x7a300000-0x7a3fffff]
           6 %D hyprframe kernel: pcieport %s: PME: Signaling with IRQ %s||Aug 19 22:28:51 hyprframe kernel: pcieport 0000:00:06.0: PME: Signaling with IRQ 126
           6 %D hyprframe kernel: %s %s: enabling device (%s -> %s)||Aug 19 22:28:51 hyprframe kernel: intel-lpss 0000:00:15.0: enabling device (0004 -> 0006)
           7 %D hyprframe kernel: e820: reserve RAM buffer [mem %d-%s]||Aug 19 22:28:51 hyprframe kernel: e820: reserve RAM buffer [mem 0x0009f000-0x0009ffff]
           7 %D hyprframe kernel: pci %s: PME# supported from D0 D3hot D3cold||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0: PME# supported from D0 D3hot D3cold
           7 %D hyprframe kernel: %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: landlock: Up and running.
           8 %D hyprframe kernel: ACPI: Dynamic OEM Table Load:||Aug 19 22:28:51 hyprframe kernel: ACPI: Dynamic OEM Table Load:
           8 %D hyprframe kernel: pci %s:   bridge window [mem %d-%s 64bit pref]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.0:   bridge window [mem 0x6000000000-0x601bffffff 64bit pref]
           8 %D hyprframe kernel: %s hash table entries: 131072 (order: 8, 1048576 bytes, linear)||Aug 19 22:28:51 hyprframe kernel: Mount-cache hash table entries: 131072 (order: 8, 1048576 bytes, linear)
          10 %D hyprframe kernel: pci %s: Adding to iommu group %d||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:07.3: Adding to iommu group 0
          10 %D hyprframe kernel: pci %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:0a.0: enabling Extended Tags
          10 %D hyprframe kernel: %s %s: %s: %s %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: vgaarb: bridge control possible
          10 %D hyprframe kernel: usbcore: registered new interface driver %s||Aug 19 22:28:51 hyprframe kernel: usbcore: registered new interface driver usbfs
          12 %D hyprframe kernel: cpuidle: using governor %s||Aug 19 22:28:51 hyprframe kernel: cpuidle: using governor ladder
          12 %D hyprframe kernel: pci %s:   bridge window [mem %d-%s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0:   bridge window [mem 0x7a300000-0x7a3fffff]
          12 %D hyprframe kernel: system 00:02: [mem %d-%s] has been reserved||Aug 19 22:28:51 hyprframe kernel: system 00:02: [mem 0xfedc0000-0xfedc7fff] has been reserved
          12 %D hyprframe kernel: %x: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: ACPI: bus type USB registered
          14 %D hyprframe kernel: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: Initialise system trusted keyrings
          17 %D hyprframe kernel: %s: %s %s %s %s||Aug 19 22:28:51 hyprframe kernel: raid6: using avx512x2 recovery algorithm
          18 %D hyprframe kernel: pci %s: PCI bridge to [bus %s]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:06.0: PCI bridge to [bus 01]
          18 %D hyprframe kernel: wlan0: deauthenticated from %m (Reason: 0=<unknown>)||Aug 20 00:22:05 hyprframe kernel: wlan0: deauthenticated from e8:1c:ba:9d:c5:e0 (Reason: 0=<unknown>)
          19 %D hyprframe kernel: ACPI: \_%s: New power resource||Aug 19 22:28:51 hyprframe kernel: ACPI: \_SB_.PC00.PEG0.PXP_: New power resource
          19 %D hyprframe kernel: usb %s: SerialNumber: 0000:00:0d.0||Aug 19 22:28:51 hyprframe kernel: usb usb1: SerialNumber: 0000:00:0d.0
          22 %D hyprframe kernel: pci %s: BAR 0 [mem %d-%s 64bit]||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:02.0: BAR 0 [mem 0x605c000000-0x605cffffff 64bit]
          24 %D hyprframe kernel: pci %s: [%s] type 00 class %s conventional PCI endpoint||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:00.0: [8086:9a14] type 00 class 0x060000 conventional PCI endpoint
          28 %D hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:a0:d3:c1:83:e3:a8:86:dd SRC=fe80:0000:0000:0000:a2d3:c1ff:fe83:e3a8 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=896 TC=0 HOPLIMIT=255 FLOWLBL=0 PROTO=UDP SPT=5353 DPT=5353 LEN=856 ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:a0:d3:c1:83:e3:a8:86:dd SRC=fe80:0000:0000:0000:a2d3:c1ff:fe83:e3a8 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=896 TC=0 HOPLIMIT=255 FLOWLBL=0 PROTO=UDP SPT=5353 DPT=5353 LEN=856
          28 %D hyprframe kernel: pci %s: PME# supported from %s %s||Aug 19 22:28:51 hyprframe kernel: pci 0000:00:0d.0: PME# supported from D3hot D3cold
          31 %D hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=fe80:0000:0000:0000:90d3:2ee5:3d79:3b30 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=%s TC=0 HOPLIMIT=255 FLOWLBL=26027 PROTO=UDP SPT=5353 DPT=5353 LEN=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=fe80:0000:0000:0000:90d3:2ee5:3d79:3b30 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=189 TC=0 HOPLIMIT=255 FLOWLBL=26027 PROTO=UDP SPT=5353 DPT=5353 LEN=149
          32 %D hyprframe kernel: usb 3-%d: new full-speed USB device number %d using xhci_hcd||Aug 19 22:28:51 hyprframe kernel: usb 3-6: new full-speed USB device number 2 using xhci_hcd
          47 %D hyprframe kernel: wlan0: association with e8:1c:ba:9d:c5:e0 timed out||Aug 19 23:06:46 hyprframe kernel: wlan0: association with e8:1c:ba:9d:c5:e0 timed out
          57 %D hyprframe kernel: wlan0: disassociated from e8:1c:ba:73:3c:00 (Reason: 8=DISASSOC_STA_HAS_LEFT)||Aug 20 01:35:58 hyprframe kernel: wlan0: disassociated from e8:1c:ba:73:3c:00 (Reason: 8=DISASSOC_STA_HAS_LEFT)
          70 %D hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=172.20.1.116 DST=224.0.0.251 LEN=238 TOS=0x00 PREC=0x00 TTL=255 ID=%s DF PROTO=UDP SPT=5353 DPT=5353 LEN=218 ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC= SRC=172.20.1.116 DST=224.0.0.251 LEN=238 TOS=0x00 PREC=0x00 TTL=255 ID=46274 DF PROTO=UDP SPT=5353 DPT=5353 LEN=218
          82 %D hyprframe kernel: wlan0: disconnect from AP %m for new auth to %m||Aug 19 22:28:51 hyprframe kernel: wlan0: disconnect from AP e8:1c:ba:9d:c5:e0 for new auth to e8:1c:ba:73:3c:00
         126 %D hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:f4:dd:06:3a:3e:b9:86:dd SRC=fe80:0000:0000:0000:f6dd:06ff:fe3a:3eb9 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=208 TC=0 HOPLIMIT=255 FLOWLBL=1032098 PROTO=UDP SPT=5353 DPT=5353 LEN=168 ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:f4:dd:06:3a:3e:b9:86:dd SRC=fe80:0000:0000:0000:f6dd:06ff:fe3a:3eb9 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=208 TC=0 HOPLIMIT=255 FLOWLBL=1032098 PROTO=UDP SPT=5353 DPT=5353 LEN=168
         156 %D hyprframe kernel: wlan0: deauthenticating from %m by local choice (Reason: 3=DEAUTH_LEAVING)||Aug 19 22:32:22 hyprframe kernel: wlan0: deauthenticating from e8:1c:ba:73:3c:00 by local choice (Reason: 3=DEAUTH_LEAVING)
         308 %D hyprframe kernel: wlan0: Limiting TX power to 21 (24 - 3) dBm as advertised by %m||Aug 19 22:28:51 hyprframe kernel: wlan0: Limiting TX power to 21 (24 - 3) dBm as advertised by e8:1c:ba:9d:c5:e0
         317 %D hyprframe kernel: wlan0: RX %s from %m (capab=0x511 status=0 aid=%d)||Aug 19 22:28:51 hyprframe kernel: wlan0: RX AssocResp from e8:1c:ba:9d:c5:e0 (capab=0x511 status=0 aid=3)
         367 %D hyprframe kernel: wlan0: authenticate with %m (local address=96:11:9b:a3:51:45)||Aug 19 22:28:51 hyprframe kernel: wlan0: authenticate with e8:1c:ba:9d:c5:e0 (local address=96:11:9b:a3:51:45)
         370 %D hyprframe kernel: wlan0: send auth to %m (try 1/3)||Aug 19 22:28:51 hyprframe kernel: wlan0: send auth to e8:1c:ba:9d:c5:e0 (try 1/3)
         501 %D hyprframe kernel: wlan0: associate with %m (try 1/3)||Aug 19 22:28:51 hyprframe kernel: wlan0: associate with e8:1c:ba:9d:c5:e0 (try 1/3)
         531 %D hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=01:00:5e:00:00:fb:a0:d3:c1:83:e3:a8:08:00 SRC=172.20.1.113 DST=224.0.0.251 LEN=%s TOS=0x00 PREC=0x00 TTL=255 ID=%s PROTO=UDP SPT=5353 DPT=5353 LEN=%s ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=01:00:5e:00:00:fb:a0:d3:c1:83:e3:a8:08:00 SRC=172.20.1.113 DST=224.0.0.251 LEN=861 TOS=0x00 PREC=0x00 TTL=255 ID=1497 PROTO=UDP SPT=5353 DPT=5353 LEN=841
         686 %D hyprframe kernel: %s: %s||Aug 19 22:28:51 hyprframe kernel: NetLabel: Initializing
        1185 %D hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:74:86:e2:26:ef:c8:86:dd SRC=fe80:0000:0000:0000:2eb1:a4f9:99ac:2395 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=92 TC=0 HOPLIMIT=1 FLOWLBL=248228 PROTO=UDP SPT=5353 DPT=5353 LEN=52 ||Aug 19 22:28:51 hyprframe kernel: nftables: dropped input: IN=enp0s13f0u2c2 OUT= MAC=33:33:00:00:00:fb:74:86:e2:26:ef:c8:86:dd SRC=fe80:0000:0000:0000:2eb1:a4f9:99ac:2395 DST=ff02:0000:0000:0000:0000:0000:0000:00fb LEN=92 TC=0 HOPLIMIT=1 FLOWLBL=248228 PROTO=UDP SPT=5353 DPT=5353 LEN=52
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
|%D | syslog time (Jan 10 00:14:59) |
|%b | BASE64 encoded data |

The template file can be used with the '-t {fname}' switch
to provide artificial ignorance capabilities.

```
% tmpltr -w ignore.templates /var/log/syslog | sort -n
```

After the above run of the tool, a file named ignore.templates
is created in the current directory as shown below:

```sh
$ cat ignore.templates
%D %s %s: %s %s: [%s] %s %d %s %s %s %s
%D %s %s: %s: %s %s %m (%s: %d=<%s>)
%D %s %s: %s: %s %c.%s %s '%s.%s: %s'
%D %s %s: %s: %s %s %s %m %s %s %s %s %m
%D %s %s: %x: %s: [%s %s]: %s %s %s %s!
%D %s %s: %s: %b %s %m %s %s %s (%s: %d=%s_%s)
%D %s %s: %s: %x: %s %s: %s
%D %s %s: ... %x-%s %x:   %d
%D %s %s: %s: %s %s %s %d %s %s %s
%D %s %s: %s %d-%d: %s %s %s %s, %s=%s, %s=%s, %s=%f
%D %s %s: %s: %s %s %s %s %s %s (%s)
%D %s %s: %x: %s: (%s %s %s %s %s)
%D %s %s: %s %s: %s %d [%s %d-%s]: %s
%D %s %s: %x-%s (%s): %s %x-%s
%D %s %s: %s: %s %s %s %s %s,%s %s %s
%D %s %s: %x: %s: %s %s: %s (%s)
%D %s %s: %s:  %s %s %s = %s
%D %s %s: %s: %s %s %s %s:%d:%d:%d:%d:%d:%s
%D %s %s: %s: %s %s %m (%s %s)
%D %s %s: %s %s: %s %s %d [%s %d-%s %s %s]: %s %s %d %s %d %s
%D %s %s: %s %s: %s,%s: %s %s %s %s [%s %s %d %s] %s %s:%s%s
%D %s %s: %s: %s %s %s %s (%x) %s %s
%D %s %s: %s %s: %s %s %s %s, %s %s %s
%D %s %s: %s_%s: %s %s %s %s %s
%D %s %s: %x: %c_%s: %s %s %s %s %s
%D %s %s: %s %s: %s %s %s %f %s %s
%D %s %s: %s %s %s: %s [%s %d-%s] (%s)
%D %s %s: %s: %s %s %s %s %s %s %s <%s>
%D %s %s:     %s=%c
%D %s %s: %s %s %s %x: %s (%s: %d, %s %s, %s)
%D %s %s:     %s=%s
%D %s %s: %s: %s %s %s %s:%d:%d:%d:%s
%D %s %s: %s %d-%d: %s: %s %d %s
%D %s %s: %s %s: %s %s(%c) %s %d %s %s
%D %s %s: %s %d %s %s %s "%s":
%D %s %s: %x: %s: %s %x: %s
%D %s %s: %x: %s %s %s %s %s
%D %s %s: %s: %s %s %s
%D %s %s: %s: %s %s %s %s (%s = %s, %s_%s = %s) %s %x %s
%D %s %s: %s %s: %s %s %s
%D %s %s: %s %s %s %s %s %s %s %s %s
%D %s %s: %x-%s %x-%s: %s %s %s %x-%s %s %s %s -%s
%D %s %s: %x: %s: %s %s [%s] (%s: %s  %s: %s  %s: %s)
%D %s %s: %s %s: %s %d [%s %d-%s]
%D %s %s: %s %s: %s: %s %s %s %s %s
%D %s %s: %s: %s %s %s %s %s
%D %s %s: %s %s: %s %s %s %s
%D %s %s: %s %d-%d: %s %s %s %s: %s=%d, %s=%d, %s=%d
%D %s %s: %s %s: %s %s %d [%s %d-%s %s]: %s
%D %s %s: %x: %s: [%s %s]: %s %s %s (-%s)
%D %s %s: %s %s: %s %s %s %s %s %s %d
%D %s %s: %s %s [%s %d-%s] %s %s %s %s
%D %s %s: %x: %s: %s: %s %s: %s, %s: %s %s
%D %s %s: %s: %x-%s %s %s %x: %s (%s %d, %s %s)
%D %s %s: %s %s: [%s] %s %s %s %s %s %s
%D %s %s: %s: `%s' %s %s %s %s %s %s %s %s %s %d %s; %s %s
%D %s %s: %s: %s %s %s.
%D %s %s: %s: %s %s=%s,%s,%s,%s,%s
%D %s %s: %s %s [%s  %d-%s] %s %s %s
%D %s %s:     %s
%D %s %s: %s %s: %s %s %s %s %s %s
%D %s %s: %s %s: %s %s %s %s %s:%s %s (%s)
%D %s %s: %x: %s %s %b %s %s %s %s %s %s. %s %s %s %s %s %s_%s %s %s %s %s
%D %s %s: %s %d-%d: %s: %s
%D %s %s: %s %s: %s %s %d [%s %d-%s %s]: %s %s %d %s %d %s
%D %s %s: %s {%d-%x-%s}: [%s]: %s_%s_%s %s, %s=%d
%D %s %s: %s %s %s %s %s [%s %s]
%D %s %s: %x: %s %s %s [%s] (%s)
%D %s %s: %s %s: %s %s %s %s, %s %s %s %d
%D %s %s: %s: %s[%s]: %s, %s[%s]:   %s
%D %s %s: %x: %s (%s) %s %s %s
%D %s %s: %s %d-%d:%f: %d %s %s
%D %s %s: %x: %s %s_%s_%s=%s, %s_%s=%s
%D %s %s: %s: %s %s %s %s (%s > %s), %s %s %s %s
%D %s %s: [%s] %s %s %s %s %s %s %d
%D %s %s: %s %s %s %s %x: %s (%s: %d, %s %s, %s)
%D %s %s: %s: %s %s %s %m %s
%D %s %s: %s_%s %d-%d:%f %x: %s '%s_%s' %s %s:%s, %s %s (%s %s), %m
%D %s %s: %s %s: %s %s %s %s %d
%D %s %s: %s: %s %s%s %s %s
%D %s %s: %s %c.%s %s '%s: %b'
%D %s %s: %s %x: %s %s %s
%D %s %s: %s: %s %s: %s=%s %s= %s= %s=%i %s=%i %s=%s %s=%s %s=%s %s=%d %s=%s %s %s=%s %s=%s %s=%s %s=%s
%D %s %s: %x: %s %s %s %s:
%D %s %s: %s %s: %s %s %s, %s=%s, %s=%s
%D %s %s: %s %s (%s) %s %s
%D %s %s: %x-%s: %s: %s
%D %s %s: %s: %s %s: %s=%s %s= %s=%m:%m:%s %s=%x:%s %s=%x:%s %s=%s %s=%d %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s
%D %s %s: %s %s %s %s %s %s %s
%D %s %s: %s %s %s %s %s  %s:%s %x:%d %x:%s %s
%D %s %s: %s %s %s %d
%D %s %s: %s: %s=%d
%D %s %s: %s_%s %d-%d:%f %x: %s %s %s
%D %s %s: %s: %s %s: %s=%s %s= %s=%m:%m:%s %s=%i %s=%i %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s
%D %s %s: %s: %s: %s %s [%s:%s,%s:%s] %s %s,%s %s %d,%s
%D %s %s: %s %d-%s %s %x-%s %s %s %s %d %s %s
%D %s %s: %s: %s %s %s %s %s %s
%D %s %s: %x: %s: %s %x: %i
%D %s %s: %s:   %s %s: %d:%d:%s
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
