
const char *convertRaceName(const char *raceName)
{
    if (strcmp(raceName, "Emilia Romagna Grand Prix") == 0)
    {
        return "Imola";
    }
    else
    {
        return raceName;
    }
}