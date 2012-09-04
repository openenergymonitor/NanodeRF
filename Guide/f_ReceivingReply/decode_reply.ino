int get_reply_data(word off)
{
  memset(line_buf,NULL,sizeof(line_buf));
  if (off != 0)
  {
    uint16_t pos = off;
    int line_num = 0;
    int line_pos = 0;
    
    // Skip over header until data part is found
    while (Ethernet::buffer[pos]) {
      if (Ethernet::buffer[pos-1]=='\n' && Ethernet::buffer[pos]=='\r') break;
      pos++; 
    }
    pos+=2;
    while (Ethernet::buffer[pos])
    {
      if (line_pos<49) {line_buf[line_pos] = Ethernet::buffer[pos]; line_pos++;} else break;
      pos++; 
    }
    line_buf[line_pos] = '\0';
    return line_pos;
  }
  return 0;
}
