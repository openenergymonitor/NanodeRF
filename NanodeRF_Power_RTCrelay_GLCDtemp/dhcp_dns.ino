

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
      
      static byte dnsip[] = {8,8,8,8};  
      ether.copyIp(ether.dnsip, dnsip);
      ether.printIp("DNS: ", ether.dnsip);
      
      if (use_hisip==true)
      {
        ether.copyIp(ether.hisip, hisip);
        dns_status = 1;          
      }
      
    } else { ethernet_error = 1; Serial.println("DHCP failed");  }  
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
