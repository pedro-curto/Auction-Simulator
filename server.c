#include <dirent.h>

/*int GetBidList(int AID, BIDLIST *list) {
    struct dirent **filelist;
    int n_entries, n_bids, len;
    char dirname[20];
    char pathname[32];

    sprintf(dirname, "AUCTIONS/%03d/BIDS/", AID);
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) // Could test for -1 since n_entries count always with . and ..
        return 0;

    n_bids = 0;
    list->no_bids = 0;
    while (n_entries--) {
        len = strlen(filelist[n_entries]->d_name);
        if (len == 10) // Discard '.', '..' and invalid filenames by size
            {
                sprintf(pathname, "AUCTIONS/%03d/BIDS/%s", AID, filelist[n_entries]->d_name);
            }
    }
}*/



