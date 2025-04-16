## Lab 8(Basic config)
### Connecting 2 PCs with  switch(same network)
1. add Pt-switch and 2 pcs
2. configure the IP for the PCs to same network 192.168.10.10/11 (int the specific PCs desktop/ip config)
3. Testing: ping \<IP address of other PC\> from command prompt of one PC

### Connecting 2 PCs with  Router
- Common 
    1. Add PCS and router and connect using bolt
    2. Add Ip addresses to the PCs 198.162.1.2(.1 for router) and 198.162.2.2
    - By UI    
        1. Add the IPV4 addresses in config/FastEthernet0/0 for router, `NOTE IP Address of the router on the respective interface is the Default Gateway address` 
        2. in router/specific interface enable port

    -By CLI
        
        ```
        
        Router#enable
        
        Router#configure terminal
    
        
        Router(config)#interface fastEthernet 1/0
        
        Router(config-if)#ip address 198.162.2.1 255.255.255.0
        
        Router(config-if)#no shutdown

## Lab 9 Implementation of Basic Topologies using PacketTracer

### Adding  Fast interfaces on switches(Pt-switch)
1. Open switch physical
2. Turn off packet tracer switch
3. add PT-SWITCH-NM-1CFE as it adds fast interface over copper corss cable
4. Turn on the packet Tracer switch

### Adding  port for router on router (1841)
1. open physical
2. switch off
3. add WIC-1ENET

`NOTE copy running-config startup-config` so that when you turn it off and on you don't have to restart it (in global scope ie exit till you reach router #)




`NOTE w.x.y.0` can not be assigned to nodes, so as standard give w.x.y.1 to router and then keep increasing the count for PCs

## Lab 10 Configuring Routing Protocols using Packet Tracer
### RIP
1. set up routers and PCS(all pcs to have a same subnet(network) on oneside of an interface of the router)
2. set the default gateway of the router as the the IPV4 address of the Fastinterface
3. set serial numbers to a diff network for each connection
4. Remember no shutdown(ie switch on port for all) in router
5. set RIP values as the subnet address of all the addresses present on the router(Fastinterface and serial)
### OSPF
1. set up routers and pcs normally
2. set areas for routers(the pc is  connected to the area of its connected router)
3. clock rate 6400 (for all ports while no shutdown) 
4. select router in CLI
    ```
        enable
        config t
        router ospf <number diff for each router 1,2,3....>
        network <Network address of the PC/Router connected to it> <Network wildcard(ie inverse of subnet)> area 0
        (above for al the connected)
        exit
    ```
### Redistribution in RIS and OSPF
1. set up RIP and OSPF on both sides and the common router
2. in CLI of common router 
    in OSPF side
    ```
    router ospf <Process number>
    redistribute rip subnets
    ```
    in RIP side
    ```
    router rip
    version 2
    no auto-summary
    redistribute ospf 10 metric 2

    ```

## Lab 11 Configuring DHCP and NAT On A Multi-Function Device
### Configure a DHCP Server
1. Connect PCs and server
2. Server: Desktop -> IP Configuration -> Set IPv4 Address only for server(like pc ie def gateway is router)
3. Server: Services -> DHCP -> Set default gateway and number of users -> Save -> Turn on
4. toogle service on
5. PC -> Desktop -> IP Configuration -> Select DHCP
6. Test connectivity

### Configure a DHCP Router
1. set up Router and PCs
2. give ip to router's ports
3. in CLI
    ```
    Router>enable
    Router#configure terminal
    Router(config)# ip dhcp pool <network name>
    Router(dhcp-config)# network <router ip address> <subnet mask>
    Router(dhcp-config)# default-router <ip address of default gateway>
    Router(dhcp-config)#dns-server <diff ip address in network>    
    ````
### static NAT


