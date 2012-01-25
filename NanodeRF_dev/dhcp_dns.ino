int dhcp_status = 0;
int dns_status = 0;

void dhcp_dns()
{
  //-----------------------------------------------------------------------------------
  // Get DHCP address
  // Putting DHCP setup and DNS lookup in the main loop allows for: 
  // powering nanode before ethernet is connected
  //-----------------------------------------------------------------------------------
  if (ether.dhcpExpired()) dhcp_status = 0;    // if dhcp expired start request for new lease by changing status
  
  if (!dhcp_status){    
    dhcp_status = ether.dhcpSetup();           // DHCP setup
    
    if (dhcp_status){                          // on success print out ip's
      ether.printIp("IP:  ", ether.myip);
      ether.printIp("GW:  ", ether.gwip);  
      
      static byte dnsip[] = {8,8,8,8};  
      ether.copyIp(ether.dnsip, dnsip);
      ether.printIp("DNS: ", ether.dnsip);
      //ether.copyIp(ether.hisip, hisip);                             // un-comment for posting to static IP server (no domain name)
      //dns_status = 1;                                               // un-comment for posting to static IP server (no domain name)            
    } else { ethernet_error = 1; }  
  }
  
  //-----------------------------------------------------------------------------------
  // Get server address via DNS
  //-----------------------------------------------------------------------------------
  if (dhcp_status && !dns_status){
    
    dns_status = ether.dnsLookup(website);    // Attempt DNS lookup
    
    if (dns_status){
      ether.printIp("SRV: ", ether.hisip);      // server ip
    } else { ethernet_error = 1; }  
  }

}
