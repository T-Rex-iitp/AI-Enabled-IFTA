//---------------------------------------------------------------------------

#ifndef SBS_MessageH
#define SBS_MessageH
#define MODES_MAX_SBS_SIZE          256
bool ModeS_Build_SBS_Message (const modeS_message *mm, TADS_B_Aircraft *a, char *msg);
bool SBS_Message_Decode( char *msg);
//---------------------------------------------------------------------------
#endif
