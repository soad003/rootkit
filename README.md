# rootkit

TODO:

Generell: Merkt an welche Dependencies euer Rootkit hat, d.h. für
welche Kernelversion das Ganze läuft. Das ist bei Rootkits verdammt
heikel, da sich die API (vor Allem nicht dokumentiere Teile davon)
verdammt schnell ändert. Auf Seite 4 habt ihr z.B. Beispielcode aus
dem 2.6er Kernel.

Geht mal ein was lsmod eigentlich genau macht und warum dann list_del
funktioniert. Wander lsmod wirklich über die ll der module? Sind die
immer an der gleichen Adresse?

Schreibt ein wenig über die Tasklet API (Referenz nicht vergessen).
Basiert die API auf SoftIRQ? Seit wann gibt es diesen support im Kernel?

Referenziert hier einen Ring3 keylogger und schreibt über den Vorteil
eines Kernel Keyloggers (1-2 Sätze reichen).

p8 netfilter library, <-- Referenz!

p10 Usage:
* Sagt noch etwas über eure Files, *.c *.h, wo findet sich was?
* Sagt noch etwas zu den compile options, make ist mir da zu wenig.
Braucht man ein spezielles Buildenvironment?

TODO ~~:

Idealerweise referenziert ihr auch die Tasklet API bzw. die
Doku wenn ihr darüber schreibt.

p3 Hier würde es sich auszahlen eigentlich auch über Ring0 bis 3 zu
reden. Damit ein Leser versteht warum es so toll ist Ring0 zu
kompromitieren.

anmerkungcle: geht e :D oder?

Ihr habt das Ganze für ASCII gemacht, wie sieht das Ganze mit einem
anderen Format aus? Z.B.  QWERTZ DIN 2137-2 (das wäre Deutschland)? Es
wäre nett zu wissen ob das Ganze zu verwenden ist wenn mein Passwort
YzÖÄäü#* ist. Wenn nein, ein zwei Satze dazu warum nicht.
