#include "accumulation.h"

void slide_window(sliding_window &window, key_map &total, key_set &significant_tiles, int time_key, int tau)
{
    key_map vals = window[time_key];
    key_map::iterator it;

    // levo da totals i dati della finestra in scadenza 
    for (it = vals.begin(); it != vals.end(); it++)
    {
        point c = it->first; // prendo la chiave di vals
        int old_count = total[c];
        total[c] -= it->second; // prendo il valore di vals
        int new_count = total[c];
        // TODO : da controllare, per adesso funziona, non si tocca...
        if (new_count <= 0)
        {
            // total[c] = 0;
            total.erase(c);
        }
        if (old_count >= tau && new_count < tau)
        {
            significant_tiles.erase(c);
        }
    }
    window.erase(time_key);
}
