# Topologije

## Krožna point-to-point topologija

Krožna point-to-point topologija (ali **topologija obroča**) opisuje omrežje, kjer je vsaka naprava povezana z natanko dvema drugima napravama v zaprti zanki.

Krožne topologije omogočajo krožni prenos podatkov med *n* vozlišči. Podatkovni paketi potujejo v eno smer in prehajajo skozi vsako vozlišče v obroču, dokler ne dosežejo cilja.

V krožnih topologijah lahko podatke hkrati prenaša le eno vozlišče. To se nadzoruje z **žetonom**, ki kroži po omrežju in omogoča posameznemu vozlišču prenos podatkov. Vozlišče ne sme prenašati podatkov brez posedovanja žetona.

### Prednosti

- Naenkrat lahko prenaša le eno vozlišče, kar zmanjšuje trke paketov (*packet collision*).
- Implementacija je pogosto preprosta; kompleksnost dodajanja več vozlišč se linearno skalira, za razliko od kvadratnega skaliranja mrežaste (*mesh*) topologije.

### Slabosti

- Če eno vozlišče odpove, se celotno omrežje izklopi.
- Zmogljivost omrežja je enostavno preobremeniti.



## Mrežasta (mesh) topologija

V omrežju z mesh topologijo je vsaka naprava povezana direktno z vsako drugo napravo.
Tako omrežje je torej decentralizirano.

### Prednosti

- Več možnih poti omogoča boljšo odpornost do napak: če povezava odpove, uberemo drugo pot
- Raztegljivo (scalable) omrežje: dodajanje naprave ne spremeni strukture obstoječega omrežja
- Učinkovit prenos podatkov: ne potujejo skozi centralno vozlišče

### Slabosti

- Kompleksna izdelava in vzdrževanje: vsaka naprava je povezana z vsako (alternativa: delno mrežasta topologija)
- Visoka cena
- Možnost redundantnih povezav

## Fat tree topologija

## Zvezdasta topologija
V omrežju z zvezdasto topologijo je vsaka naprava povezana na skupno centralno vozlišče (hub)
Ločimo aktivno in pasivno zvezdasto topologijo.
Pri aktivni centralno vozlišče pošlje prejeto sporočilo napravam v omrežju, pri pasivni pa centralno vozlišče služi le kot povezava.

### Prednosti
- Odpornost do napak
- Eliminira trke paketov
- Cenovno ugodno: vsaka naprava potrebuje le ena vrata za komunikacijo
- Raztegljivo: dodajanje naprave ne spremeni strukture obstoječega omrežja
- Enostavna diagnoza napak

### Slabosti
- Centralno vozlišče je točka odpovedi
- Zmogljivost odvisna od centralnega vozlišča

## Spine leaf topologija
Je topologija, sestavljena iz dveh slojev stikal: leaf in spine
Uporabljena je v glavnem v podatkovnih centrih, kjer imamo opravka z velikim številom naprav.
Je alternativa tradicionalni troslojni arhitekturi
Leaf: stikala, ki prejemajo podatke
Spine: stikala, ki leaf sloj povezujejo v mrežasto topologijo

### Prednosti
- Redundanca stikal
- Raztegljivost
- Nizka latenca v primerjavi s tradicionalno

### Slabosti
- Potrebujemo več stikal
- Za veliko uporab prekompleksna topologija
