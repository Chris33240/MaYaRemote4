#include "infrared_sender_RC6_toggle.h"

/// Envoie une trame RC6 avec gestion automatique du bit de bascule (toggle).
///
/// Cette version conserve en interne l’état du toggle, qui est inversé à
/// chaque appel.
///
/// @param IrSender Instance de l’émetteur IR.
/// @param command Commande RC6 à envoyer.
/// @param nbits Nombre de bits de la commande.
/// @param toggle_bit_mask Masque indiquant la position du bit de bascule.
void sendRC6Toggle(IRsend IrSender, uint64_t command, short nbits, uint32_t toggle_bit_mask)
{
  static int toggle = 0;
  if (toggle == 0)
  {
    IrSender.sendRC6(command, nbits);
  }
  else
  {
    IrSender.sendRC6(command ^ toggle_bit_mask, nbits);
  }
  toggle = 1 - toggle;
}

/// Envoie une trame RC6 avec gestion externe du bit de bascule (toggle).
///
/// Cette version n’entretient pas d’état interne : l’appelant doit fournir
/// explicitement la valeur du toggle (0 ou 1).
///
/// @param IrSender Instance de l’émetteur IR.
/// @param command Commande RC6 à envoyer.
/// @param nbits Nombre de bits de la commande.
/// @param toggle_bit_mask Masque indiquant la position du bit de bascule.
/// @param toggle Valeur du bit de bascule (0 = non activé, 1 = activé).
void sendRC6Toggle(IRsend IrSender, uint64_t command, short nbits, int toggle_bit_mask, uint32_t toggle)
{
  if (toggle == 0)
  {
    IrSender.sendRC6(command, nbits);
  }
  else
  {
    IrSender.sendRC6(command ^ toggle_bit_mask, nbits);
  }
}