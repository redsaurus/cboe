/*
 *  fileio.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>

void init_fileio();

struct no_file_chosen {}; // an exception class

Boolean scen_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode);
Boolean party_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode);
FSSpec nav_get_scenario() throw(no_file_chosen);
FSSpec nav_put_scenario() throw(no_file_chosen);
bool load_scenario(FSSpec file_to_load);
bool load_town(short which_town, cTown*& the_town);
bool load_town(short which_town, cSpeech& the_talk);
bool load_town_str(short which_town, short which_str, char* str);
bool load_town_str(short which_town, cTown*& the_town);
bool load_outdoors(location which_out,cOutdoors& the_out);
bool load_outdoors(location which_out, short mode, ter_num_t borders[4][50]);
bool load_outdoor_str(location which_out, short which_str, char* str);
void load_spec_graphics();
std::vector<std::string> load_strings(std::string which);

FSSpec nav_get_party() throw(no_file_chosen);
FSSpec nav_put_party() throw(no_file_chosen);
bool load_party(FSSpec file_to_load);
bool save_party(FSSpec dest_file);
