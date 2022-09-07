//SendClientMessage Separation v1.0

#if !defined PAWNREGEX_INC_
#error Pawn.Regex include not found
#endif

stock SendClientMessage_Separation(playerid, color, const message[])
{
	if(strlen(message) > 144)
	{
		new HexColor[9];
		//»щем Hex цвета в строке
		new Regex:rg_FindHexColor = Regex_New("\\{[a-zA-Z0-9]{6}\\}");
		new RegexMatch:match, pos, startpos;
		while(Regex_Search(message, rg_FindHexColor, match, pos, startpos))
		{
			new length;
			Match_GetGroup(match, 0, HexColor, length);
			startpos += pos + length;
			Match_Free(match);
		}
		Regex_Delete(rg_FindHexColor);
		//////////////////////////////
		pos = 0;
		new str[145];
		while(pos < strlen(message))
		{
			str[0] = EOS;
			strmid(str, message, pos, pos+141);

			if(pos)
			{
				strins(str, "...", 0);
				if(strlen(HexColor)) strins(str, HexColor, 0);
			}

			pos += 141;
			if(pos < strlen(message)) strcat(str, "...");
			SendClientMessage(playerid, color, str);
			printf("%s", str);
		}
		return 1;
	}
	else return SendClientMessage(playerid, color, message);
}
#if defined _ALS_SendClientMessage
	#undef SendClientMessage
#else
	#define _ALS_SendClientMessage
#endif
#define SendClientMessage SendClientMessage_Separation
