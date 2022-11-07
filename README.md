# RPi + ED900

Jeu de fléchettes connecté avec affichage sur matrice LED 128×64.

## Matériel

- [Decathlon Canaveral ED 900](https://www.decathlon.fr/p/cible-de-flechettes-electronique-connectee-ed-900/_/R-p-300600)
- Raspberry Pi (testé avec Raspberry Pi Zero 2 W)
- [Adafruit RGB Matrix Bonnet](https://www.adafruit.com/product/3211)
- Matrice LED 128×64 (ou deux matrices 64×64) de type Hub75
- Alimentation 5V (MeanWell LRS-100-5 ou LRS-50-5)
- Boutons arcade rétro-éclairés + contrôleur USB

## Dépendances

- `libsystemd`
- `libpthread`
- `libpng`
- `libevdev`
- [`rpi-rgb-led-matrix`](https://github.com/hzeller/rpi-rgb-led-matrix) ou `libsdl2`

## Compatibilité

Les cibles [Dartslive Home](https://www.dartslive.com/fr/dartslivehome/) devraient pouvoir être adaptées sans difficulté majeure.

