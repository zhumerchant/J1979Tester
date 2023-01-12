# J1979Tester
Diagnose vehicle based on J1979 standard. Functions include reading VIN, DTC, and parameter value.
Support following protocols:
  1. ISO15765(Standard and extended frame)
  2. KWP14230
  3. ISO9141
  4. J1850 VPW
  5. J1850 PWM
# Build
The program is built with Visual Studio 2022. It contains 3 projects: J1979Tester, duilib and CChart.
The project duilib and CChart are refered by project J1979Tester.
# Installation
The software communication layer is based on J2534 passthru standard, so must install J2534 passthru driver
before running J1979Tester. 
Currently support 2 types J2534 devices: 
  1. GODIAG J2534 (Amazon link: https://a.co/d/eArjdUR)
  2. VNCI Nano
# Multiple language support
The software supports 23 languages
