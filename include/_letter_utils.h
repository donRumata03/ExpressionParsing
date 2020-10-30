//
// Created by Vova on 05.07.2020.
//

#pragma once

/// Language recognition:
inline bool is_russian_letter_(const char c) { // C is in cp1251!
	constexpr unsigned char russian_A = 192;
	constexpr unsigned char russian_a = 224;
	constexpr char russian_ch_A = char(russian_A);
	constexpr char russian_ch_a = char(russian_a);

	constexpr unsigned char russian_Ya = 223;
	constexpr unsigned char russian_ya = 255;
	constexpr char russian_ch_Ya = char(russian_Ya);
	constexpr char russian_ch_ya = char(russian_ya);

	constexpr unsigned char russian_Yo = 168;
	constexpr unsigned char russian_yo = 184;
	constexpr char russian_ch_Yo = char(russian_Yo);
	constexpr char russian_ch_yo = char(russian_yo);

	return (russian_ch_A <= c  && c <= russian_ch_Ya) || (russian_ch_a <= c  && c <= russian_ch_ya) || (c == russian_ch_Yo) || (c == russian_ch_yo);
}
inline bool is_english_letter_(const char c){
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline bool is_letter_(const char c){
	return is_russian_letter_(c) || is_english_letter_(c);
}

inline bool is_digit_(const char c){
	// return (c >= '1' && c <= '9') || c == '0';
	return (c >= '0' && c <= '9');
}

inline bool is_space_(const char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}



