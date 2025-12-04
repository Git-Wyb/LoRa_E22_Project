git@github.com:Git-Wyb/LoRa_E22_Project.git

E22 LoRa样机

功率10 -> 实测8.2dBm
功率13 -> 实测11dBm

发射频率922.2MHz,实测偏了2K，21ppm误差
SF5-9 BW125KHz  可以。SF9，速率实测2.44Kbps
SF10  BW125不行，250KHz可以
SF11  BW125、250不行，500KHz可以

定点发射：模块地址和网络地址相同，才能通信
广播发射：模块地址0xFF 0xFF，所有信道以内都可以通信。