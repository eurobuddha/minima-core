#pragma once
#include <string>
namespace minima {
// Exact pinned multisig.MULTISIG_CONTRACT; the command itself is not yet ported.
inline const std::string MULTISIG_CONTRACT = "LET root=PREVSTATE(1) IF root NEQ 0x21 THEN IF SIGNEDBY(root) THEN RETURN TRUE ENDIF ENDIF LET n=PREVSTATE(2) LET m=PREVSTATE(3) LET script=[RETURN MULTISIG(]+STRING(n) LET counter=0 WHILE counter LT m DO LET script=script+[ ]+STRING(PREVSTATE(counter+4)) LET counter=INC(counter) ENDWHILE LET script=script+[)] EXEC script";
}
