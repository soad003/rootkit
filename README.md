# rootkit

TODO:

Generell: Merkt an welche Dependencies euer Rootkit hat, d.h. für
welche Kernelversion das Ganze läuft. Das ist bei Rootkits verdammt
heikel, da sich die API (vor Allem nicht dokumentiere Teile davon)
verdammt schnell ändert. Auf Seite 4 habt ihr z.B. Beispielcode aus
dem 2.6er Kernel.

Referenzen: Wenn ihr auf eine Webseite zeigt, dann muss die URL
drinnen sein und wann ihr die Seite besucht habt: Visited, March,
2013. Wenn die URL zu lange ist dann nehmt von mir aus tinyurl oder
ähnlich. Idealerweise referenziert ihr auch die Tasklet API bzw. die
Doku wenn ihr darüber schreibt.


Seite/Kommentar

p1 Logo der Uni



p3 Hier würde es sich auszahlen eigentlich auch über Ring0 bis 3 zu
reden. Damit ein Leser versteht warum es so toll ist Ring0 zu
kompromitieren.

p4 "That means even something as small as a single bad pointer could
possibly wipe your hard drive."

Recht dramatisch, ganz so schlimm ist es nicht, wenn ihr nicht gerade
mit dem Filesystem spielt. Was euch passieren kann und wird, dass ihr
euch euer System zerschießts, darum könnt ihr gleich anmerken, dass es
sich auszahlt Rootkits in einer VM zu coden oder auf UML zu switchen.

p5 "To prevent our module from the most obvious way of detection we
want to hide it from lsmod ."

Löscht das "we want to hide it", das klingt sehr nach einem Tutorial
aus dem Netz. Schreibt lieber, "To prevent the module from being
detected via a common means of detection (i.e. lsmod) measures are
taken, such as ..."

Geht mal ein was lsmod eigentlich genau macht und warum dann list_del
funktioniert. Wander lsmod wirklich über die ll der module? Sind die
immer an der gleichen Adresse?


"The hard part was not the creation of the process itself but doing it
inside of an interrupt handler (reception of magic packet). Many
functions can not be called safely (long running functions and so on)
inside the context of an interrupt"


Schreibt ein wenig über die Tasklet API (Referenz nicht vergessen).
Basiert die API auf SoftIRQ? Seit wann gibt es diesen support im Kernel?

p7 Keylogging

Habt ihr das aus einem Buch? Bennent evtl. eure Quellen.

Referenziert hier einen Ring3 keylogger und schreibt über den Vorteil
eines Kernel Keyloggers (1-2 Sätze reichen).

Ihr habt das Ganze für ASCII gemacht, wie sieht das Ganze mit einem
anderen Format aus? Z.B.  QWERTZ DIN 2137-2 (das wäre Deutschland)? Es
wäre nett zu wissen ob das Ganze zu verwenden ist wenn mein Passwort
YzÖÄäü#* ist. Wenn nein, ein zwei Satze dazu warum nicht.


p8 netfilter library, <-- Referenz!


p9 2.4.2 UPD Sockets
To send the keys to the attacker UPD datagram packets are used.


Massive Platzverschwendung. Eine Section, subsection etc. sollte
mindestends ein paar Sätze lang sein. 
Warum habt ihr UDP Pakete verwendet und nicht TCP (hint: drops aber
keine fixe verbindung)? Was sind UDP Pakete? Wie sind diese aufgebaut?

p10 Usage:
* Sagt noch etwas über eure Files, *.c *.h, wo findet sich was?
* Sagt noch etwas zu den compile options, make ist mir da zu wenig.
Braucht man ein spezielles Buildenvironment?