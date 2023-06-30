Proiectul #6 Utilitar pentru monitorizarea la distanță a mai multor sisteme Linux:

Monitorizare:
* consum de resurse(RAM; CPU; I/O; temperatură): top, htop sau sar 
* colectare informații în mod regulat și stocare într-un fisier

* Utilizatori
* timpi de autentificare:  fisierele /var/log/auth.log
* ssh: 
* servicii active: service

* dispozitive hardware: lspci, lsusb sau dmidecode 
* starea hardware-ului și pentru a detecta eventuale probleme sau erori.

Acțiuni remote
* oprire/pornire servicii/procese: systemctl + ZOOM (informatii suplimentare procese)
* copiere fișiere: scp
* instalare aplicații/servicii: Ansible?

var/syslog
socket
etc/shadow

erori de autentificare? ssh, cauza

zoom in pe serviciu (proc)

thread pool pentru socket

forced data