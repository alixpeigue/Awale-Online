# Jeu d'Awale en ligne

## Instructions de compilation :

`make` pour compiler, produit deux éxécutables :
 - le serveur `./srv.out`
 - le client `./cli.out`

## Lancer le jeu

Lancer le serveur `./srv.out` puis le client `./cli.out <adresse-ip-du-serveur>`

## Quitter

A tout moment, vous pouvez saisir `/close` pour fermer le client.

## Utiliser le client

A la connexion, le client demande votre nom d'utilisateur, c'est ce qui vous identifie auprès du serveur et des autres personnes.

Vous avez ensuite plusieurs choix.

### Créer un salon

Le jeu vous donne un code que vous pouvez partager pour inviter vos amis, puis vous place dans le salon en attendant l'arrivée d'autres personnes.

### Rejoindre un salon

Si un ami vous a partagé le code d'un salon, pous pouvez le rejoindre en saisissant le code, s'il y a déja deux joueurs dans le salon, vous ne pourrez pas le rejoindre en tant que joueur, essayez de le rejoindre en tant que spectateur.

### Observer un salon

Pour observer une partie dans un salon, saisissez le code de ce salon.

### Saisir votre biographie

Vous pouvez donner une biographie afin que les gens en sachent plus sur vous quand ils rejoignent une partie dans laquelle vous êtes.

### Déroulement d'un salon

Lorsqu'il n'y a pas encore deux joueurs, les spectateurs peuvent échanger des messages entre eux ou avec le joueur en attente d'un adversaire.

Lorsque la partie commence, les joueurs doivent saisir tour à tour leurs coups en entrant la position. Les joueurs peuvent toujours envoyer des messages : si ce qu'ils saisissent n'est pas un nombre alors c'est considéré comme un message.
Les spectateurs peuvent aussi continuer à envoyer des messages.

Vous pouvez également quitter le salon en entrant `/leave`.
Quitter un salon en tant que joueur ferme le salon (pas en tant que spectateur).
