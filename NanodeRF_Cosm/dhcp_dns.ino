

//static byte hisip[] = { 192,168,10,2 };    // un-comment for posting to static IP server (no domain name)            

void dhcp_dns()
{
  //-----------------------------------------------------------------------------------
  // Get DHCP address
  // Putting DHCP setup and DNS lookup in the main loop allows for: 
  // powering nanode before ethernet is connected
  //-----------------------------------------------------------------------------------
  //if (!ether.dhcpSetup()) dhcp_status = 0;    // if dhcp expired start request for new lease by changing status
  
  if (!dhcp_status){
    
    #ifdef UNO
    wdt_disable();
    #endif 
    
    dhcp_status = ether.dhcpSetup();           // DHCP setup
    
    #ifdef UNO
    wdt_enable(WDTO_8S);
    #endif
    
    Serial.print("DHCP status: ");             // print
    Serial.println(dhcp_status);               // dhcp status
    
    if (dhcp_status){                          // on success print out ip's
      ether.printIp("IP:  ", ether.myip);
      ether.printIp("GW:  ", ether.gwip);  
      
      ether.printIp("DNS: ", ether.dnsip);
      
//      ether.copyIp(ether.hisip, hisip);                             // un-comment for posting to static IP server (no domain name)
//      dns_status = 1;                                               // un-comment for posting to static IP server (no domain name)            
    } else { ethernet_error = 1; }  
  }
  
  //-----------------------------------------------------------------------------------
  // Get server address via DNS
  //-----------------------------------------------------------------------------------
  if (dhcp_status && !dns_status){
    
    #ifdef UNO
    wdt_disable();
    #endif 
    
    dns_status = ether.dnsLookup(website);    // Attempt DNS lookup
    
    #ifdef UNO
    wdt_enable(WDTO_8S);
    #endif;
    
    Serial.print("DNS status: ");             // print
    Serial.println(dns_status);               // dns status
    if (dns_status){
      ether.printIp("SRV: ", ether.hisip);      // server ip
    } else { ethernet_error = 1; }  
  }

}
