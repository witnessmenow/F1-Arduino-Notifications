
const char *convertRaceName(const char *raceName)
{
    if (strcmp(raceName, "Emilia Romagna Grand Prix") == 0)
    {
        return "Imola";
    }
    else if (strcmp(raceName, "Barcelona-Catalunya") == 0)
    {
        return "Barcelona";
    }
    else
    {
        return raceName;
    }
}