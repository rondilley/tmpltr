#!/bin/bash
#
# Generate test data for tmpltr testing
#

echo "Generating test data files..."

# Create data directory if it doesn't exist
mkdir -p data

# Basic log file
cat > data/basic.log << 'EOF'
2024-01-15 10:23:45 INFO User john logged in from 192.168.1.100
2024-01-15 10:24:12 INFO User jane logged in from 192.168.1.101
2024-01-15 10:25:33 ERROR Failed login attempt from 192.168.1.102
2024-01-15 10:26:45 INFO User john logged out from 192.168.1.100
2024-01-15 10:27:00 ERROR Failed login attempt from 192.168.1.103
EOF

# Quoted strings log
cat > data/quoted.log << 'EOF'
Request "GET /index.html" from 192.168.1.1
Request "POST /api/login" from 192.168.1.2
Response "200 OK" sent to 192.168.1.1
Response "401 Unauthorized" sent to 192.168.1.2
Request "GET /style.css" from 192.168.1.1
EOF

# Integer values
cat > data/integers.log << 'EOF'
Process 1234 started
Process 5678 stopped
Memory usage: 2048 MB
CPU usage: 75 percent
Process 9012 started
EOF

# Float values
cat > data/floats.log << 'EOF'
Temperature: 23.5 degrees
Humidity: 65.8 percent
Pressure: 1013.25 hPa
Temperature: 24.1 degrees
Humidity: 64.2 percent
EOF

# Hex values
cat > data/hex_values.log << 'EOF'
Memory address: 0x7fff5fbff8c0
Error code: 0xDEADBEEF
Register value: 0x0000CAFE
Memory address: 0x7fff5fbff8d0
Error code: 0xBADC0DE
EOF

# Mixed case hex (for case consistency test)
cat > data/hex_mixed_case.log << 'EOF'
Value: 0xDeAdBeEf should be string
Value: 0xDEADBEEF should be hex
Value: 0xdeadbeef should be hex
Value: 0xBaDc0De should be string
EOF

# MAC addresses
cat > data/mac_addresses.log << 'EOF'
Device MAC: 00:11:22:33:44:55
Router MAC: AA:BB:CC:DD:EE:FF
Client MAC: 12:34:56:78:9A:BC
Device MAC: 00:11:22:33:44:66
Router MAC: AA:BB:CC:DD:EE:00
EOF

# IP addresses
cat > data/ip_addresses.log << 'EOF'
Connection from 192.168.1.1
Connection from 10.0.0.1
Connection from 172.16.0.1
Connection from 192.168.1.2
Connection from 10.0.0.2
EOF

# BASE64 data
cat > data/base64.log << 'EOF'
Packetdata:SGVsbG8gV29ybGQh
Packetdata:SGVsbG8gV29ybGQ=
Packetdata:SGVsbG8gV29ybA==
Packetdata:dGVzdCt0ZXN0L3Rlc3Q=
Packetdata:3KYyaJoq1Haglac3CABFAAAoibNAACsG2bGi8zVGCgoKKJgqFwxQgSHNhD6Jg1AR+vCZMAAAAAAAAAAADg==
EOF

# Syslog dates
cat > data/syslog_dates.log << 'EOF'
Jan 15 10:23:45 server01 sshd[1234]: Connection accepted
Feb  3 09:15:22 server02 httpd[5678]: Request received
Mar 21 14:30:00 server01 sshd[1234]: Connection closed
Apr  1 00:00:01 server03 cron[9012]: Job started
Dec 31 23:59:59 server02 httpd[5678]: Shutting down
EOF

# Syslog position test
cat > data/syslog_position.log << 'EOF'
Jan 15 10:23:45 server sshd: This should be syslog date
NotJan 15 10:23:45 server sshd: This should NOT be syslog date
 Jan 15 10:23:45 server sshd: This should NOT be syslog date (leading space)
EOF

# ISO timestamps
cat > data/iso_timestamps.log << 'EOF'
2024-01-15T10:23:45Z Event occurred
2024-02-03T09:15:22+00:00 Another event
2024-03-21T14:30:00-05:00 Timezone event
2024-04-01T00:00:01Z Start of day
2024-12-31T23:59:59Z End of year
EOF

# Empty file
touch data/empty.log

# Long lines (4KB lines)
perl -e 'for($i=0;$i<10;$i++){print "Line $i: "."A"x4000 ."\n"}' > data/long_lines.log

# Binary mixed data
echo -e "Normal text\x00\x01\x02Binary data\nMore text" > data/binary_mixed.log

# Template ignore file
cat > data/ignore_templates.txt << 'EOF'
%t INFO User %s logged in from %i
%t ERROR Failed login attempt from %i
EOF

# Performance test data (100k lines)
echo "Generating performance test data (100k lines)..."
perl -e '
@users = qw(john jane bob alice charlie);
@ips = map {"192.168.1.$_"} (1..255);
@actions = ("logged in", "logged out", "failed login", "password changed");
for($i=0; $i<100000; $i++) {
    $user = $users[rand @users];
    $ip = $ips[rand @ips];
    $action = $actions[rand @actions];
    $time = sprintf("2024-01-15 %02d:%02d:%02d", int(rand(24)), int(rand(60)), int(rand(60)));
    print "$time INFO User $user $action from $ip\n";
}' > data/perf_100k.log

echo "Test data generation complete!"