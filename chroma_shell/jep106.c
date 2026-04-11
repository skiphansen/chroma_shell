#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static const char *JEP106_LookupTbl[][0x7e] = {
#include "jep106.inc"
};

static bool CheckParity(uint8_t Data,bool bOdd)
{
   bool bIsOdd = false;
   uint8_t Mask = 1;

   for(int i = 0; i < 8; i++) {
      if(Data & Mask) {
         bIsOdd = !bIsOdd;
      }
      Mask <<= 1;
   }
   return bOdd == bIsOdd;
}

bool JEP106_ID_2_string(
   uint8_t *pData,
   int DataLen,
   uint8_t *pDevId,
   uint16_t *pManId,
   const char **Desc)
{
   uint8_t ManufactureID = 0;
   int Bank = 0;
   bool Ret = false; // Assume the worse
   const char *RetDesc = "Unknown";
   const char *ParityErr = "Invalid - parity error";
   const char *BankErr = "Invalid - bank";

   for(int i = 0; i < DataLen; i++) {
      if((ManufactureID = pData[i]) == 0x7f) {
         Bank++;
         if(Bank > 16) {
            RetDesc = BankErr;
            break;
         }
      }
      else {
      // found bank, check parity
         if(!CheckParity(ManufactureID,true)) {
            RetDesc = ParityErr;
            break;
         }
         Ret = true;
         ManufactureID &= 0x7f;
         if(JEP106_LookupTbl[Bank][ManufactureID - 1] != NULL) {
            RetDesc = JEP106_LookupTbl[Bank][ManufactureID - 1];
         }
         if(pManId != NULL) {
            *pManId = ManufactureID | (Bank << 8);
         }
         if(pDevId != NULL && i < DataLen - 1) {
            *pDevId = pData[i + 1];
         }
         break;
      }
   }

   if(Desc != NULL) {
      *Desc = RetDesc;
   }

   return Ret;
}
