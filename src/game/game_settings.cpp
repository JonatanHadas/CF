#include "game_settings.h"

#include "../utils/serialization.h"

GameSettings::GameSettings(
	ScoreSettings scores,
	bool using_teams,
	const vector<string>& team_names,
	const vector<int>& teams,
	const set<PowerUpDescriptor>& allowed_powerups,
	const vector<string>& names,
	const vector<int>& colors
) :
	scores(scores),
	using_teams(using_teams),
	team_names(team_names),
	teams(teams),
	allowed_powerups(allowed_powerups),
	names(names),
	colors(colors) {}
	
int GameSettings::get_team_num() const {
	return (using_teams ? team_names : names).size();
}

vector<int> GameSettings::get_teams() const {
	if(using_teams){
		return teams;
	}
	vector<int> non_teams;
	for(int i = 0; i < names.size(); i++){
		non_teams.push_back(i);
	}
	return non_teams;
}

void GameSettings::serialize(ostream& output) const {
	scores.serialize(output);
	
	write_raw(output, using_teams);
	
	write_vector<string>(output, team_names, write_string);
	write_vector<int>(output, teams, write_raw<int>);
	
	write_vector<PowerUpDescriptor>(output,
		vector<PowerUpDescriptor>(
			allowed_powerups.begin(),
			allowed_powerups.end()
		),
		write_serializable<PowerUpDescriptor>
	);
	
	write_vector<string>(output, names, write_string);
	write_vector<int>(output, colors, write_raw<int>);
}

GameSettings GameSettings::deserialize(istream& input){
	auto scores = ScoreSettings::deserialize(input);
	
	auto using_teams = read_raw<bool>(input);
	
	auto team_names = read_vector<string>(input, read_string);
	auto teams = read_vector<int>(input, read_raw<int>);
	
	auto allowed_powerups = read_vector<PowerUpDescriptor>(input, PowerUpDescriptor::deserialize);
	
	auto names = read_vector<string>(input, read_string);
	auto colors = read_vector<int>(input, read_raw<int>);
	
	return GameSettings(
		scores,
		using_teams,
		team_names,
		teams,
		set<PowerUpDescriptor>(
			allowed_powerups.begin(),
			allowed_powerups.end()
		),
		names,
		colors
	);
}
