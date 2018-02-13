\mainpage Tsensor codename

\todo 1. DHT nas portas A0, A1 e A2 (lacava)

# LOG alteracao

## V 0.55
1. Ethernet, sensor seco, sensor analogico e DHT  (cx 0.41  postos SIM)
2. Removido DNS da biblioteca UIP 
	+ http://michaelnoland.com/reducing-code-size-on-arduino-ethernet-boards/

## V 0.53

1. acerto no sensor seco para interrupção, PCINT2, PCINT1, PCINT0, USE_SENSOR_SECO_BAIXO
2. acerto na memoria id_timestamp que grava sobre o MD5 (gParam)
3. ao salvar ssid/senha no wifi, já conecta a rede.
	+ #define F_SETUP_WIFI 8 // configurou wifi via browser
100. BUGS
	+ ler nivel do sinal wifi rssi

## Histórico
1. versao 0.22 CAP 23/5/15
2. versao com TS/DO (DigitalOcean)
3. versao 0.3 CAP 22/7/15   -wifi firmware 1.2.0, 115200bauds
4. versao 0.31 CAP 22/7/15   
5.	removido graficos
6. versao 0.31 CAP 13/10/15
7. removido &status
8. versão ESP8266 1.4 19/10/15
9. Incluido HAS_SENSOR_ANALOGIACO 20/03/2016
10. Incluido HAS_IR_REMOTE 5/4/2016
11. Incluido multi sensores DS20B18 20/5/2016
12. desativado o t_reboot (sATCIPSTARTMultiple)  v0.38
13. criado no gParam.chave para MD5 25/05/2016 v.038
14. pedir chave para gravar, mostrar seq no webserver 25/05/2016 v.039 TS0 7
15. criado define DEMO, atualmente para mostrar chave MD5  27/05/2016 v039
16. flagHTTP  bit F_RET_HTTP se =1 erro no retorno 27/05/2016  v0.39
17. #define PERMITER_TROCAR_xxxx criado 30/05/2016 v 0.40
18. AT+CWJAP trocado string p/ WIFI [GOT] IP e espera OK 04/06/2016 v0.40
19. removido P_STATUS_RESET_FABRICA, marcado com //1 05/06/2016 v 0.41 firmware 1.5.4 
20. removido envio field1, somado no servidor os field2+filed3+field4 05/06/2016 v0.41
21. alterado bits de conexão wifi 06/06/2016 v0.41
22. criado i2c master e funcional  v0.42 10/06/2016
23. I2c e ethernet funcional v 0.43 24/06/2016  (erro no wifi)
24. rele ok, e modbus (beta) v 0.44
25. aumentado de 5 para 8 numero de sensores remotos em modbus
26. sensor_seco 0.46
27. RTC 3231, AT24C23 4KB eeprom
28. 047 HAS_BLUETOOTH
29. Versao 048 : sensor fumaca = sensor analogico
30. Versão 051 : sensor_sensor baixo e alto, 2 sensores de fumaça

