# Pomembne datoteke

- src -> Node.cc
- src -> Node.ned

- simulations -> omnetpp.ini
- simulations -> ringnetwork.ned

## Navodila

Projekt je mogoče ponovno ustvariti z osnovnem setup in le z vključitvijo zgoraj omenjenih datotek.

# Opis uporabljenih gradnikov (modulov in kanalov)

V analiziranem zgledu krožne P2P topologije so bili uporabljeni osnovni gradniki simulacijskega okolja OMNeT++, ki omogočajo modeliranje omrežnih vozlišč, komunikacijskih povezav in časovnega poteka dogodkov. Model ne uporablja ogrodja INET, temveč temelji na lastno definiranih modulih in standardnih OMNeT++ kanalih.

## Modul node

```
simple Node
```
Modul `Node` predstavlja posamezno omrežno vozlišče v krožni P2P topologiji.

### Funkcionalnost

Vsak modul:
* sprejema sporočila prek vhodnih vrat (in[]),
* obdeluje sporočila v metodi handleMessage(),
* posreduje sporočila naslednjemu vozlišču v krogu prek izhodnih vrat (out[]).

| Metoda             | Opis                                                                                          |
| ------------------ | --------------------------------------------------------------------------------------------- |
| `initialize()`     | Inicializacija ID-ja vozlišča, postavitev v krožno obliko, inicializacija začetnega sporočila |
| `handleMessage()`  | Sprejem in obdelava sporočil                                                                  |
| `forwardMessage()` | Posredovanje sporočila naslednjemu vozlišču                                                   |

## Vrata (Gates)

```
gates:
    input in[];
    output out[];
```

### Namen
* omogočajo razširljivo povezovanje poljubnega števila vozlišč,
* omogočajo uporabo zank `for` v definiciji povezav,
* podpirajo avtomatsko indeksiranje povezav.

## Kanal DatarateChannel

## Tip kanala
```
DatarateChannel { datarate = 100Mbps; delay = 1ms; }
```

Kanal `DatarateChannel` modelira fizično komunikacijsko povezavo med dvema vozliščema z omejeno pasovno širino in zakasnitvijo.

### Parametri
| Parameter  | Opis                              |
| ---------- | --------------------------------- |
| `datarate` | Hitrost prenosa podatkov (bit/s)  |
| `delay`    | Propagacijska zakasnitev povezave |

### Model zakasnitve
`t = delay + (velikost paketa / datarate)`

## Omrežni modul circularTopology

```
network circularTopology
```

Ta modul predstavlja celotno omrežje in vsebuje:
* vektorski seznam vozlišč node[numNodes],
* definicijo krožnih povezav med vozlišči.

### Topološka struktura
Povezave so definirane z zanko:
```
for i=0..numNodes-1 {
    node[i].out++ --> DatarateChannel --> node[(i+1)%numNodes].in++;
}
```

## Sporočila in paketi

### cMessage (sporočilo)
`cMessage` predstavlja osnovni dogodek v OMNeT++ in se uporablja za diskretne dogodke. V tem modelu se `cMessage` uporablja kot preprost prenos podatkov med vozlišči, kadar ni potrebe po simulaciji dejanske velikosti paketa.

Vsako sporočilo nosi parametre, ki določajo njegovo pot:
* `hops` – število prehodov vozlišč, ki jih je sporočilo prešlo.
* `startId` – ID vozlišča, ki je poslalo sporočilo.
* `destinationId` – ID vozlišča, na katerega je sporočilo namenjeno.

### cPacket
`cPacket` je razširjena različica `cMessage`, ki omogoča simulacijo resničnih paketov omrežnega prometa, vključno z velikostjo. V tem modelu se uporablja kot paket `RingPacket`, ki omogoča:
* Nastavitev velikosti v bajtih z `setByteLength()`.
* Shranjevanje istih parametrov kot sporočila (`hops`, `startId`, `destinationId`, `startTime`).

Uporaba paketov omogoča simulacijo omrežne propustnosti in časa prenosa, saj kanali (npr. `DatarateChannel`) upoštevajo velikost paketa pri izračunu zakasnitve.

### Signali
Registracija signala `delaySignal` omogoča merjenje časov potovanja paketov/sporočil od izvora do cilja. Signal se emituje, ko sporočilo ali paket doseže cilj ali se vrne do začetnega vozlišča.























