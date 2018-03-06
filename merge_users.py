#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# This program takes a list of userdb files and merges them together.
# Fields for each DMR ID found in later files take precedence.
# In other words, as each file is processed, a field for a DMR ID is
# updated if that field is non-empty. Non-empty fields always replace
# fields from previously-processed files.  An empty field never replaces
# an existing field.

# Optionally, several other fixups are performed.  See the options below.

# Author: Dale Farnsworth dale@farnsworth.org

# MIT License
#
# Copyright 2018 Dale Farnsworth
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

from __future__ import print_function

import sys
import argparse
import shlex

version = "1.0.3++"

optionList = [
	dict(
		name = "abbrevCountries",
		default = True,
		help = "Abbreviate country names.",
	),
	dict(
		name = "abbrevDirections",
		default = True,
		help = "Abbreviate directions.",
	),
	dict(
		name = "abbrevStates",
		default = True,
		help = "Abbreviate state and province names.",
	),
	dict(
		name = "fixRomanNumerals",
		default = True,
		help = "Fix case on roman numerals. Changes mixed-case " +
			"roman numerals at the end of the name field into " +
			"upper case.",
	),
	dict(
		name = "fixStateCountries",
		default = True,
		help = "Fix US state name found in country field.",
	),
	dict(
		name = "miscChanges",
		default = True,
		help = "Do Miscellaneous cleanups.",
	),
	dict(
		name = "removeCallFromNick",
		default = True,
		help = "Remove callsign from nickname field",
	),
	dict(
		name = "removeDupSurnames",
		default = True,
		help = "Remove duplicated surname found at the end of the " +
			"name field.",
	),
	dict(
		name = "removeMatchingNick",
		default = True,
		help = "Remove nicknames that are the same as the first name " +
			"in the name field.",
	),
	dict(
		name = "removeRepeats",
		default = True,
		help = "Remove duplicated phrase in a field. If a field " +
			"consists entirely of a duplicated phrase, " +
			"one copy is removed.",
	),
	dict(
		name = "titleCase",
		default = True,
		help = "Words consisting of all capital letters are examined " +
			"and converted to title case if appropriate.",
	),
	dict(
		name = "header",
		default = True,
		help = "Prefix the list with its byte count. The first line " +
			"of the output file will contain a count of the " +
			"remaining bytes in the file.",
	),

	dict(
		name = "removeNames",
		default = False,
		help = "Remove personal names for privacy.",
	),
]

countryAbbrevs = {
	"Andorra":			"AD",
	"Argentina":			"AR",
	"Australia":			"AU",
	"Austria":			"AT",
	"Barbados":			"BB",
	"Belgium":			"BE",
	"Belize":			"BZ",
	"Bosnia and Hercegovina":	"BA",
	"Brazil":			"BR",
	"Bulgaria":			"BG",
	"Canada":			"CA",
	"Chile":			"CL",
	"China":			"CN",
	"Colombia":			"CO",
	"Costa Rica":			"CR",
	"Croatia":			"HR",
	"Cyprus":			"CY",
	"Czech Republic":		"CZ",
	"Denmark":			"DK",
	"Dominica":			"DM",
	"Dominican Republic":		"DO",
	"Ecuador":			"EC",
	"Estonia":			"EE",
	"Finland":			"FI",
	"France":			"FR",
	"Germany":			"DE",
	"Greece":			"GR",
	"Guatemala":			"GT",
	"Haiti":			"HT",
	"Hong Kong":			"HK",
	"Hungary":			"HU",
	"India":			"IN",
	"Indonesia":			"ID",
	"Ireland":			"IE",
	"Israel":			"IL",
	"Italy":			"IT",
	"Japan":			"JP",
	"Korea":			"KR",
	"Kuwait":			"KW",
	"Latvia":			"LV",
	"Liechtenstein":		"LI",
	"Luxembourg":			"LU",
	"Macedonia":			"MK",
	"Malaysia":			"MY",
	"Malta":			"MT",
	"Mexico":			"MX",
	"Montenegro":			"ME",
	"Netherlands Antilles":		"AN",
	"Netherlands":			"NL",
	"New Zealand":			"NZ",
	"Norway":			"NO",
	"Panama":			"PA",
	"Paraguay":			"PY",
	"Philippines":			"PH",
	"Poland":			"PL",
	"Portugal":			"PT",
	"Qatar":			"QA",
	"Reunion":			"RE",
	"Romania":			"RO",
	"Russia":			"RU",
	"Serbia":			"RS",
	"Singapore":			"SG",
	"Slovakia":			"SK",
	"Slovenia":			"SI",
	"South Africa":			"ZA",
	"Spain":			"ES",
	"Sweden":			"SE",
	"Switzerland":			"CH",
	"Taiwan":			"TW",
	"Thailand":			"TH",
	"Trinidad and Tobago":		"TT",
	"Turkey":			"TR",
	"Ukraine":			"UA",
	"United Kingdom":		"UK",
	"United States":		"US",
	"Uruguay":			"UY",
	"Venezuela":			"VE",
}

alternateCountryAbbrevs = {
	"Argentina Republic":		"AR",
	"Bosnia and Hercegovi":		"BA",
	"DEU":				"DE",
	"Korea S":			"KR",
	"Luxemburg":			"LU",
}

inverseCountryAbbrevs = {
	"DEU":				"Germany",
}

stateAbbrevsByCountry = {
	"United States": {
		"Alabama":		"AL",
		"Alaska":		"AK",
		"Arizona":		"AZ",
		"Arkansas":		"AR",
		"California":		"CA",
		"Colorado":		"CO",
		"Connecticut":		"CT",
		"Delaware":		"DE",
		"District of Columbia": "DC",
		"Florida":		"FL",
		"Georgia":		"GA",
		"Hawaii":		"HI",
		"Idaho":		"ID",
		"Illinois":		"IL",
		"Indiana":		"IN",
		"Iowa":			"IA",
		"Kansas":		"KS",
		"Kentucky":		"KY",
		"Louisiana":		"LA",
		"Maine":		"ME",
		"Maryland":		"MD",
		"Massachusetts":	"MA",
		"Michigan":		"MI",
		"Minnesota":		"MN",
		"Mississippi":		"MS",
		"Missouri":		"MO",
		"Montana":		"MT",
		"Nebraska":		"NE",
		"Nevada":		"NV",
		"New Hampshire":	"NH",
		"New Jersey":		"NJ",
		"New Mexico":		"NM",
		"New York":		"NY",
		"North Carolina":	"NC",
		"North Dakota":		"ND",
		"Ohio":			"OH",
		"Oklahoma":		"OK",
		"Oregon":		"OR",
		"Pennsylvania":		"PA",
		"Puerto Rico":		"PR",
		"Rhode Island":		"RI",
		"South Carolina":	"SC",
		"South Dakota":		"SD",
		"Tennessee":		"TN",
		"Texas":		"TX",
		"Utah":			"UT",
		"Vermont":		"VT",
		"Virginia":		"VA",
		"Washington":		"WA",
		"West Virginia":	"WV",
		"Wisconsin":		"WI",
	},
	"Canada": {
		"Alberta":		"AB",
		"British Columbia":	"BC",
		"Manitoba":		"MB",
		"New Brunswick":	"NB",
		"Newfoundland":		"NL",
		"Nova Scotia":		"NS",
		"Ontario":		"ON",
		"Prince Edward Island":	"PE",
		"Quebec":		"QC",
		"Saskatchewan":		"SK",
		"Yukon":		"YT",
	},
	"Australia": {
		"Australian Capital Territory": "ACT",
		"New South Wales":		"NSW",
		"Northern Territory":		"NT",
		"Queensland":			"QLD",
		"South Australia":		"SA",
		"Tasmania":			"TAS",
		"Victoria":			"VIC",
		"Western Australia":		"WAU",
	},
	"Germany": {
		"Baden-Wuerttemberg":		"BW",
		"Bavaria":			"BY",
		"Berlin":			"BE",
		"Brandenburg":			"BB",
		"Bremen":			"HB",
		"Hamburg":			"HH",
		"Hessen":			"HE",
		"Lower Saxony":			"NI",
		"Mecklenburg-Vorpommern":	"MV",
		"North Rhine-Westphalia":	"NW",
		"Rheinland-Pfalz":		"RP",
		"Saarland":			"SL",
		"Saxony":			"SN",
		"Saxony-Anhalt":		"ST",
		"Schleswig-Holstein":		"SH",
		"Thuringia":			"TH",
	},
	"Netherlands": {
		"Drenthe":			"DR",
		"Flevoland":			"FD",
		"Friesland":			"FR",
		"Gelderland":			"GE",
		"Groningen":			"GR",
		"Limburg":			"LI",
		"North Brabant":		"N-B",
		"North Holland":		"N-H",
		"Oost-Vlaanderen":		"O-V",
		"Overijssel":			"OV",
		"South Holland":		"ZH",
		"Utrecht":			"UTR",
		"Zeeland":			"ZE",
	},
	"Belgium": {
		"Antwerp":			"VAN",
		"East Flanders":		"VOV",
		"Flemish Brabant":		"VBR",
		"Hainaut":			"WHT",
		"Ile-de-France":		"IF",
		"Liege":			"WLG",
		"Limburg":			"VLI",
		"Luxembourg":			"WLX",
		"Namur":			"WNA",
		"Walloon Brabant":		"WBR",
		"West Flanders":		"VWV",
	},
	"Poland":{
		"Dolnoslaskie":			"PLDS",
		"Kujawsko-pomorskie":			"PLKP",
		"Lubelskie":			"PLLU",
		"Lubuskie":			"PLLB",
		"Lodzkie":			"PLLD",
		"Malopolskie":			"PLMA",
		"Mazowieckie":			"PLMZ",
		"Opolskie":			"PLOP",
		"Podkarpackie":			"PLPK",
		"Podlaskie":			"PLPD",
		"Pomorskie":			"PLPM",
		"Slaskie":			"PLSL",
		"Swietokrzyskie":			"PLSK",
		"Warminsko-mazurskie":			"PLWN",
		"Wielkopolskie":			"PLWP",
		"Zachodniopomorskie":			"PLZP",
	},
	"Italy": {
		"Abruzzo":			"ABR",
		"Basilicata":			"BAS",
		"Calabria":			"CAL",
		"Campania":			"CAM",
		"Emila-Romagna":			"EMI",
		"Friuli-Venezia Giulia":			"FRI",
		"Lazio":			"LAZ",
		"Liguria":			"LIG",
		"Lombardia":			"LOM",
		"Marche":			"MAR",
		"Molise":			"MOL",
		"Piemonte":			"PIE",
		"Puglia":			"PUG",
		"Sardegna":			"SAR",
		"Sicilia":			"SIC",
		"Toscana":			"TOS",
		"Trentino-Alto Adige":			"TRE",
		"Umbria":			"UMB",
		"Valle d'Aosta":			"VAL",
		"Veneto":			"VEN",
	},
}

alternateStateAbbrevsByCountry = {
	"Germany": {
		"Baden-Wurttemberg":		"BW",
		"Nordrhein-Westfalen":		"NW",
		"Rhineland-Palatinate":		"RP",
	},
	"Netherlands": {
		"Noord-Brabant":		"N-B",
		"Noord-Holland":		"N-H",
		"Zuid-Holland":			"ZH",
	},
	"Belgium": {
		"Antwerpen":			"VAN",
		"Henegouwen":			"WHT",
		"le-de-France":			"IF",
		"Namen":			"WNA",
		"Vlaams-Brabant":		"VBR",
		"West-Vlaanderen":		"VWV",
	},
	"Poland": {
		"Lesser Poland Voivod":			"PLMA",
		"Lower Silesian Voivo":			"PLDS",
		"Lublin Voivodeship":			"PLLU",
		"Lubusz Voivodeship":			"PLLB",
		"Maeopolskie":			"PLMA",
		"Maopolskie":				"PLMA",
		"Masovian Voivodeship":			"PLMZ",
		"Kuyavian-Pomeranian":			"PLPM",
		"Opole Voivodeship":			"PLOP",
		"Podkarpackie Voivode":			"PLPK",
		"Podlaskie Voivodeshi":			"PLPD",
		"Pomeranian Voivodesh":			"PLPM",
		"Silesian Voivodeship":			"PLDS",
		"Silesian Lowlands":			"PLDS",
		"Wojewudztwo Udzkie":			"PLLD",
	},
	"Italy": {
		"Valle d Aosta":			"VAL",
		"Aosta Valley":			"VAL",
		"Lombardy":			"LOM",
		"Piedmont":			"PIE",
		"Sardinia":			"SAR",
		"Savona":			"SV",
		"Tuscany":			"TOS",

	}
}

directionAbbrevs = {
	"North":		"N.",
	"South":		"S.",
	"East":			"E.",
	"West":			"W.",
}

titleCaseWords = [
	"AALBORG",
	"AARON",
	"ABEL",
	"ABY",
	"ACACIO",
	"ACHAZ",
	"ACHILLEAS",
	"AD",
	"ADA",
	"ADAM",
	"ADELIO",
	"ADRIAN",
	"ADRIANO",
	"AGUSTIN",
	"AHOSKIE",
	"AICHI",
	"AITOR",
	"AKAA",
	"ALAIN",
	"ALAM",
	"ALAN",
	"ALBERT",
	"ALBERTO",
	"ALBUQUERQUE",
	"ALCORCON",
	"ALDO",
	"ALEJANDRO",
	"ALESSANDRA",
	"ALESSANDRO",
	"ALEX",
	"ALEXANDRE",
	"ALEXANDROS",
	"ALFA",
	"ALFONSO",
	"ALFRED",
	"ALFREDO",
	"ALI",
	"ALISTER",
	"ALL",
	"ALLAN",
	"ALLIGAND",
	"ALONSO",
	"ALOSHYN",
	"ALTO",
	"ALVADOR",
	"ALVARO",
	"AMARO",
	"AMATEUR",
	"AMBERG",
	"AMEDEO",
	"AMIRED",
	"ANAHEIM",
	"ANASTASIOS",
	"ANCHISE",
	"ANDERSEN",
	"ANDERSON",
	"ANDRE",
	"ANDREA",
	"ANDREAS",
	"ANDRES",
	"ANDREW",
	"ANDRZEJ",
	"ANDY",
	"ANGEL",
	"ANGELES",
	"ANGELO",
	"ANGELOS",
	"ANIBAL",
	"ANJEL",
	"ANNA",
	"ANTHONY",
	"ANTONINO",
	"ANTONIO",
	"ANTONIOS",
	"ANTONY",
	"AOSTA",
	"ARA",
	"ARACAJU",
	"ARALA",
	"ARANDAS",
	"ARC",
	"AREA",
	"AREC",
	"ARES",
	"ARI",
	"ARIS",
	"ARISTIDIS",
	"ARITZ",
	"ARLINGTON",
	"ARNAUD",
	"ARON",
	"ARROW",
	"ARTHUR",
	"ARTUR",
	"ARTURO",
	"ASCANIO",
	"ASHEBORO",
	"ASHEVILLE",
	"ASIS",
	"ASOCCULTURAL",
	"ASSN",
	"ATA",
	"ATILLA",
	"ATLANTIC",
	"AUCKLAND",
	"AUGUSTO",
	"AURORA",
	"AUXCOMM",
	"AZORES",
	"BAGARMOSSEN",
	"BAK",
	"BAKERSVILLE",
	"BALANICI",
	"BALBINO",
	"BALDWIN",
	"BAR",
	"BARBARA",
	"BARCELONA",
	"BARIS",
	"BARRY",
	"BARTLESVILLE",
	"BARTOLOMEO",
	"BARTOSZ",
	"BATIN",
	"BAUD",
	"BAUTISTA",
	"BAY",
	"BEACH",
	"BEAR",
	"BEAT",
	"BEAVER",
	"BECHMANN",
	"BELLINGHAM",
	"BELLMORE",
	"BELO",
	"BEN",
	"BENEDETTO",
	"BENITO",
	"BENJAMIN",
	"BENNAS",
	"BENNY",
	"BENOIT",
	"BERKLEY",
	"BERNARD",
	"BERNARDINA",
	"BERNARDO",
	"BERND",
	"BERT",
	"BETHANY",
	"BEX",
	"BIAGIO",
	"BIRMINGHAM",
	"BISHOPS",
	"BIXBY",
	"BLOCKED",
	"BOBAN",
	"BOCA",
	"BODEN",
	"BOELL",
	"BOJAN",
	"BOLINGBROOK",
	"BOLL",
	"BOLOGNA",
	"BOLZANO",
	"BORTA",
	"BOSSY",
	"BOWMANVILLE",
	"BRAMPTON",
	"BRANFORD",
	"BRETT",
	"BRIAN",
	"BRIDGETON",
	"BRISTOL",
	"BROKEN",
	"BRONX",
	"BROOKEVILLE",
	"BROOKLYN",
	"BROWNS",
	"BROWNSBURG",
	"BRUNO",
	"BRYAN",
	"BULL",
	"BULLI",
	"BUNN",
	"BURCAK",
	"BURHAN",
	"BURLINGTON",
	"BUSAN",
	"BUSH",
	"CADET",
	"CAETANO",
	"CALDAS",
	"CALDEROLI",
	"CALGARY",
	"CALHOUN",
	"CAMARILLO",
	"CAMBRIDGE",
	"CAMDEN",
	"CAMP",
	"CAMPAZI",
	"CANADA",
	"CANCUN",
	"CANDIDO",
	"CAPENA",
	"CARAGIULO",
	"CARDONA",
	"CARIDAD",
	"CARINOLA",
	"CARLA",
	"CARLO",
	"CARLOS",
	"CARMELO",
	"CARPINELLI",
	"CARROLLTON",
	"CARUARU",
	"CARUGATE",
	"CASANOVAS",
	"CASCADE",
	"CASTAIC",
	"CASTALIAN",
	"CASTOR",
	"CATALIN",
	"CATANIA",
	"CATHERINE",
	"CAXIAS",
	"CD",
	"CEDRIC",
	"CELESTINO",
	"CENK",
	"CENTENNIAL",
	"CESAR",
	"CESARE",
	"CHANDLER",
	"CHANG",
	"CHARLES",
	"CHAVES",
	"CHENG",
	"CHEONG",
	"CHESTER",
	"CHEYENNE",
	"CHIA",
	"CHICAGO",
	"CHINO",
	"CHOCTAW",
	"CHRISOSTOMOS",
	"CHRISTCHURCH",
	"CHRISTIAN",
	"CHRISTOPHE",
	"CHRISTOPHER",
	"CHRISTOS",
	"CHULA",
	"CHURCH",
	"CHYUAN",
	"CIESZYN",
	"CINCO",
	"CIRO",
	"CITY",
	"CIVERA",
	"CLACKAMAS",
	"CLARKSVILLE",
	"CLAUDE",
	"CLAUDIO",
	"CLAUS",
	"CLETO",
	"CLIFTON",
	"CLINTON",
	"CLIVE",
	"CLUB",
	"CO",
	"COAST",
	"CODINA",
	"COLIN",
	"COLOGNE",
	"COLON",
	"COLORADO",
	"COLUMBIA",
	"COMANO",
	"COMO",
	"CONCORD",
	"CONQUENSES",
	"CONSUELO",
	"CONTAGEM",
	"COQUIMBO",
	"CORDIGNANO",
	"CORDSVILLE",
	"CORNER",
	"CORONA",
	"COSIMO",
	"COURSE",
	"COVENTRY",
	"COX",
	"CRAIG",
	"CREEK",
	"CREWE",
	"CRISNA",
	"CRISTIAN",
	"CRISTIANO",
	"CRISTOBAL",
	"CROSS",
	"CROWN",
	"CRUZ",
	"CTY",
	"CUNEYT",
	"CUTLER",
	"DAEGU",
	"DAI",
	"DALDOSS",
	"DALLAS",
	"DALTON",
	"DAMIANO",
	"DAMON",
	"DANIEL",
	"DANIELE",
	"DANNEMORA",
	"DANTE",
	"DANY",
	"DARIO",
	"DARREN",
	"DAVE",
	"DAVID",
	"DAVIDE",
	"DAVY",
	"DE",
	"DEAN",
	"DEBRAY",
	"DEKALB",
	"DELSBO",
	"DELTA",
	"DEMETRIO",
	"DEMIS",
	"DEMO",
	"DENIS",
	"DENIZ",
	"DENNI",
	"DENNIS",
	"DEON",
	"DERECK",
	"DEREK",
	"DES",
	"DI",
	"DIAMOND",
	"DIEGO",
	"DIFONA",
	"DIMITRIOS",
	"DIMITRIS",
	"DIMOS",
	"DINNINGTON",
	"DIVINOPOLIS",
	"DNEPROPETROVSK",
	"DO",
	"DOBBS",
	"DOLOMITES",
	"DOMENEC",
	"DOMENICO",
	"DOMINGA",
	"DOMINGO",
	"DOMINIC",
	"DONATO",
	"DONCASTER",
	"DORIN",
	"DORME",
	"DORR",
	"DOSQUET",
	"DOWNINGTOWN",
	"DRUMMONDVILLE",
	"DUEPRE",
	"DUNCAN",
	"DUNEDIN",
	"DUQUE",
	"DURHAM",
	"DURSUN",
	"DX",
	"EAST",
	"EASTLAKE",
	"EDITH",
	"EDMONTON",
	"EDOARDO",
	"EDUARDO",
	"EDWARD",
	"EFRAIM",
	"EFSTRATIOS",
	"EG",
	"EGUNA",
	"EL",
	"ELETHERIA",
	"ELIGIO",
	"ELIZABETH",
	"ELKHART",
	"ELMIRA",
	"ELOEM",
	"ELSINORE",
	"EMANUELE",
	"EMBRO",
	"EMILE",
	"EMILIANO",
	"EMILIO",
	"EMMANOUIL",
	"ENMORE",
	"ENNIO",
	"ENON",
	"ENRIC",
	"ENRICO",
	"ENRIQUE",
	"ENZO",
	"EOC",
	"EPSOM",
	"ERCOLE",
	"ERGUL",
	"ERHAN",
	"ERIC",
	"ERIKSSON",
	"ERIO",
	"ERMELLINO",
	"EROL",
	"ERRANTE",
	"ERVO",
	"ERWIN",
	"ESBJERG",
	"ESCUELA",
	"ESER",
	"ESLOV",
	"ESSEX",
	"ESTEBAN",
	"ESTER",
	"ETTERS",
	"ETTORE",
	"EUCLID",
	"EUGENIO",
	"EVAGGELOS",
	"EVANSVILLE",
	"EVERGREEN",
	"EVGENI",
	"EWA",
	"EZIO",
	"FABIO",
	"FABRETTI",
	"FABRICE",
	"FABRIZIO",
	"FALCON",
	"FALLS",
	"FARMINGVILLE",
	"FARSTA",
	"FAUSTO",
	"FCO",
	"FEDERICA",
	"FEDERICO",
	"FELIPE",
	"FELIX",
	"FERDINANDO",
	"FERDY",
	"FERHAT",
	"FERNANDO",
	"FERRARA",
	"FERRY",
	"FIDEL",
	"FIDENZA",
	"FILIPPO",
	"FILOMENO",
	"FINSPNG",
	"FL",
	"FLAT",
	"FLAVIO",
	"FLEN",
	"FLIX",
	"FLORENCE",
	"FLORENTINO",
	"FLORIANO",
	"FOKION",
	"FORMIGINE",
	"FORSHAGA",
	"FORT",
	"FORTUNATO",
	"FOTIOS",
	"FOUNTAIN",
	"FRACCHIOLLA",
	"FRANCA",
	"FRANCESCA",
	"FRANCESCO",
	"FRANCIS",
	"FRANCISACO",
	"FRANCISCO",
	"FRANCO",
	"FRANCOIS",
	"FRANK",
	"FRANKLIN",
	"FRANKLINVILLE",
	"FRED",
	"FREDERIC",
	"FREITAS",
	"FRESNO",
	"FRIEDEN",
	"FUENTE",
	"FULTON",
	"FUNKERFREUNDE",
	"GABRIEL",
	"GABRIELE",
	"GALANTE",
	"GALBUSERA",
	"GALLATIN",
	"GARANHUNS",
	"GARCIA",
	"GARDNERVILLE",
	"GARETH",
	"GARIBALDI",
	"GARRY",
	"GARY",
	"GENNARO",
	"GEOFFREY",
	"GEOFFROY",
	"GEORGE",
	"GEORGIOS",
	"GERARD",
	"GERARDINO",
	"GERARDO",
	"GERBER",
	"GERLANDO",
	"GERMAN",
	"GERMANTOWN",
	"GERVASIO",
	"GIACINTO",
	"GIAN",
	"GIANCARLO",
	"GIANDOMENICO",
	"GIANFRANCO",
	"GIANLUCA",
	"GIANNI",
	"GIANNIS",
	"GILDO",
	"GILLES",
	"GINES",
	"GINO",
	"GIORGIO",
	"GIORGOS",
	"GIOVANNI",
	"GIRAY",
	"GIUBIASCO",
	"GIULIA",
	"GIUSEPPE",
	"GIVRINS",
	"GLASGOW",
	"GLASTONBURY",
	"GLIGA",
	"GLYN",
	"GODOWA",
	"GOFFREDO",
	"GONZALO",
	"GOOSE",
	"GORDON",
	"GOTHENBURG",
	"GOTTFRIED",
	"GRADO",
	"GRAHAM",
	"GRAND",
	"GRANDE",
	"GRANGE",
	"GRANITEVILLE",
	"GRAZIANO",
	"GREENBACKVILLE",
	"GREENSBORO",
	"GREGORIO",
	"GRIMSBY",
	"GRNQVIST",
	"GROUP",
	"GROVE",
	"GROVU",
	"GRUFTIZ",
	"GRUNDSUND",
	"GRUYERE",
	"GRZEGORZ",
	"GU",
	"GUADALAJARA",
	"GUANGLIN",
	"GUATEMALA",
	"GUAXUPE",
	"GUERINO",
	"GUIDO",
	"GUILLERMO",
	"GUIRGIO",
	"GUSTAVO",
	"GUTHRIE",
	"GUY",
	"HABRA",
	"HADONG",
	"HAINESVILLE",
	"HAKAN",
	"HAMDEN",
	"HAMILTON",
	"HAMMOND",
	"HAMPSTEAD",
	"HARBOR",
	"HARCO",
	"HARMAN",
	"HARRISON",
	"HARRY",
	"HARTFORD",
	"HATYAI",
	"HAWTHORNE",
	"HAYES",
	"HECTOR",
	"HEEB",
	"HELDER",
	"HELLAS",
	"HENDERSON",
	"HENDRIK",
	"HENRICO",
	"HENRYK",
	"HERMINIO",
	"HERMON",
	"HERVE",
	"HEYNSMANS",
	"HICKORY",
	"HILL",
	"HILLIARD",
	"HILLS",
	"HILO",
	"HJELM",
	"HONG",
	"HOPEDALE",
	"HOPEWELL",
	"HORIZONTE",
	"HOSCHTON",
	"HOUSTON",
	"HOVMANTORP",
	"HQ",
	"HRISTO",
	"HUDDINGE",
	"HUGHES",
	"HUGO",
	"HUGUES",
	"HUMBERTO",
	"HUNT",
	"HUNTINGTON",
	"IAIN",
	"IAN",
	"ICHUAN",
	"IEROTHEOS",
	"IGINO",
	"IGNACIO",
	"IHOR",
	"IHSAN",
	"IINGNACIO",
	"ILION",
	"ILKESTON",
	"ILLAPEL",
	"IMPERO",
	"IMRE",
	"INDIANAPOLIS",
	"INN",
	"IOAN",
	"IOANNIS",
	"ION",
	"IONIA",
	"IOSEBA",
	"IPER",
	"IRAKLIS",
	"IRENEO",
	"IRVINE",
	"ISAAC",
	"ISIDORO",
	"ISIDRO",
	"ISLIP",
	"ISTANBUL",
	"IVAN",
	"IVANO",
	"IVO",
	"IVOR",
	"JACEK",
	"JACK",
	"JACKSON",
	"JACKSONVILLE",
	"JACKY",
	"JACQUES",
	"JACQUOT",
	"JAIME",
	"JAMES",
	"JAMIE",
	"JANAUBA",
	"JANEIRO",
	"JANEZ",
	"JAN",
	"JANOS",
	"JANUSZ",
	"JASON",
	"JAUME",
	"JAVIER",
	"JEAN",
	"JEJU",
	"JENKINSVILLE",
	"JENSEN",
	"JEROME",
	"JESS",
	"JESUS",
	"JIM",
	"JOAN",
	"JOAO",
	"JOAQUIM",
	"JOAQUIN",
	"JOE",
	"JOEL",
	"JOHAN",
	"JOHANN",
	"JOHN",
	"JOHNNY",
	"JOISE",
	"JON",
	"JORDI",
	"JORGE",
	"JOSE",
	"JOSEBA",
	"JOSEF",
	"JOSEP",
	"JOSEPH",
	"JOSU",
	"JOZSEF",
	"JP",
	"JR",
	"JUAN",
	"JUANMA",
	"JUDICAEL",
	"JULIAN",
	"JULIEN",
	"JULIO",
	"JUREK",
	"JUSTO",
	"KADIR",
	"KAESER",
	"KAILUA",
	"KALIX",
	"KAMIENICA",
	"KANATA",
	"KANE",
	"KAPITI",
	"KARA",
	"KARG",
	"KARL",
	"KARLSBORG",
	"KARLSTAD",
	"KARST",
	"KATARZYNA",
	"KEANSBURG",
	"KEITH",
	"KELVIN",
	"KEMPISCHE",
	"KEN",
	"KENT",
	"KEVIN",
	"KEY",
	"KIL",
	"KINGSTON",
	"KINGWOOD",
	"KIRKLAND",
	"KISSIMMEE",
	"KLAASSE",
	"KLAUS",
	"KLUB",
	"KNOXVILLE",
	"KOKOMO",
	"KONG",
	"KONSTANTINOS",
	"KORAY",
	"KORIA",
	"KOSTAS",
	"KOUMALA",
	"KRIS",
	"KROTKOFALOWCOW",
	"KRZYSZTOF",
	"KYIV",
	"KYOTO",
	"LA",
	"LAFAYETTE",
	"LAGUNA",
	"LAILA",
	"LAKE",
	"LAKELAND",
	"LAKEWOOD",
	"LANDER",
	"LANDVETTER",
	"LANOKA",
	"LAPEER",
	"LARA",
	"LAS",
	"LATISANA",
	"LAURENT",
	"LAURO",
	"LAWRENCE",
	"LAWTON",
	"LAZARO",
	"LEANDRO",
	"LEDYARD",
	"LEEN",
	"LEFTERIS",
	"LEGNAGO",
	"LELLI",
	"LENOIR",
	"LEON",
	"LEONARD",
	"LEONARDI",
	"LEONARDO",
	"LESLIE",
	"LESO",
	"LEVENT",
	"LEWISBERRY",
	"LEXINGTON",
	"LIANI",
	"LILLIAMO",
	"LINCOLN",
	"LINDENHURST",
	"LINO",
	"LISSAIOS",
	"LITTLE",
	"LIVONIA",
	"LODI",
	"LOMBARD",
	"LORANI",
	"LORENZO",
	"LOS",
	"LOUDONVILLE",
	"LOUISBURG",
	"LOUISVILLE",
	"LOYOLA",
	"LUBLIN",
	"LUC",
	"LUCA",
	"LUCIA",
	"LUCIANO",
	"LUCIO",
	"LUETHY",
	"LUIGI",
	"LUIS",
	"LUISA",
	"LUKASZ",
	"LULEA",
	"LURATI",
	"LYE",
	"MACIEK",
	"MADISON",
	"MADISONVILLE",
	"MAERTENS",
	"MAGNIER",
	"MAHWAH",
	"MALCOLM",
	"MANAHAWKIN",
	"MANCHESTER",
	"MANFRED",
	"MANHATTAN",
	"MANOLO",
	"MANOS",
	"MANTORP",
	"MANUEL",
	"MARACAIBO",
	"MARC",
	"MARCEL",
	"MARCELLA",
	"MARCELLO",
	"MARCIN",
	"MARCO",
	"MARCOS",
	"MARCUS",
	"MARGATE",
	"MARIA",
	"MARIAN",
	"MARIE",
	"MARINA",
	"MARINO",
	"MARIO",
	"MARK",
	"MARKUS",
	"MARRA",
	"MARTIAL",
	"MARTIN",
	"MARTINEZ",
	"MARTINO",
	"MARYS",
	"MASSAPEQUA",
	"MASSIMILIANO",
	"MASSIMO",
	"MASTIC",
	"MATEO",
	"MATOUS",
	"MATS",
	"MATTEO",
	"MATTIA",
	"MAUMEE",
	"MAURIZIO",
	"MAURO",
	"MAXI",
	"MAYQUEL",
	"MC",
	"MCDONOUGH",
	"MCLOUGHLIN",
	"MCNAIRY",
	"MEDFORD",
	"MEGERDICH",
	"MEHMET",
	"MEINHARD",
	"MELBOURNE",
	"MEMPHIS",
	"MERIDA",
	"MERIDIAN",
	"MERRICK",
	"MERRILLVILLE",
	"MESA",
	"MEZIMESTI",
	"MIAMI",
	"MICHAEL",
	"MICHALIS",
	"MICHEL",
	"MICHELE",
	"MICK",
	"MIDDLETON",
	"MIDLAND",
	"MIDLOTHIAN",
	"MIDWEST",
	"MIGUEL",
	"MIKAEL",
	"MIKE",
	"MIKEL",
	"MILANO",
	"MILES",
	"MILFORD",
	"MILLS",
	"MILOCIN",
	"MILTIADIS",
	"MINDEN",
	"MINEOLA",
	"MINOZZI",
	"MIRA",
	"MIROSLAW",
	"MISHAWAKA",
	"MITCHELL",
	"MITROI",
	"MOMCILO",
	"MONROE",
	"MONTEBELLO",
	"MONTGOMERY",
	"MOR",
	"MORE",
	"MORENO",
	"MORGANTON",
	"MORRISTOWN",
	"MORUMBI",
	"MOUNT",
	"MOUNTAIN",
	"MOUNTVILLE",
	"MURALTO",
	"MUSKOGEE",
	"MUSTANG",
	"MUZAFFER",
	"MYERS",
	"NA",
	"NAGY",
	"NANDO",
	"NAPOLI",
	"NARINO",
	"NATCHEZ",
	"NATHALIE",
	"NATIONAL",
	"NAZZARENO",
	"NEILS",
	"NEW",
	"NEWARK",
	"NEWINGTON",
	"NEWTOWN",
	"NEZIH",
	"NG",
	"NICHELINO",
	"NICK",
	"NICOLA",
	"NICOLAOS",
	"NIGEL",
	"NIKO",
	"NIKOLAOS",
	"NIKOLAS",
	"NIKOS",
	"NISOLLE",
	"NITRO",
	"NJURUNDA",
	"NO",
	"NOEL",
	"NOELLE",
	"NOKOMIS",
	"NORBERTO",
	"NORSBORG",
	"NORTH",
	"NORWICH",
	"NSW",
	"NUEVE",
	"NUNZIO",
	"NURI",
	"NWS",
	"NYLUND",
	"OAK",
	"OAKLEY",
	"OCEANA",
	"OCEANSIDE",
	"OESCH",
	"OGDENSBURG",
	"OGICA",
	"OH",
	"OKLAHOMA",
	"OLATHE",
	"OLE",
	"OLIVERO",
	"OLIVIER",
	"OLNEY",
	"OLYMPIA",
	"OLYPHANT",
	"OMAR",
	"ON",
	"ONANCOCK",
	"ONOFRIO",
	"ONZ",
	"OO",
	"ORESTE",
	"ORIAGO",
	"ORLAND",
	"ORLANDO",
	"ORSA",
	"ORTIZ",
	"OSBY",
	"OSCAR",
	"OSLO",
	"OSNY",
	"OSWEGO",
	"OTAKI",
	"OTTAVIO",
	"OTTAWA",
	"OULU",
	"OVALLE",
	"OVERLAND",
	"OVIDIU",
	"OVIEDO",
	"PABLO",
	"PACIFICO",
	"PACO",
	"PALHOCA",
	"PALM",
	"PALMARES",
	"PANAGIOTIS",
	"PANAMA",
	"PANTELEIMON",
	"PAOLA",
	"PAOLO",
	"PARK",
	"PARMA",
	"PARMELE",
	"PARRAVICINI",
	"PARVIZ",
	"PASADENA",
	"PASCAL",
	"PASCHALIS",
	"PASCUAL",
	"PASQUALE",
	"PASQUALINO",
	"PASTOR",
	"PATRICK",
	"PATXI",
	"PAUL",
	"PAULINE",
	"PAULO",
	"PAWEL",
	"PEDRO",
	"PENANG",
	"PENDERGRASS",
	"PER",
	"PEREZ",
	"PERRY",
	"PERTH",
	"PERUGIA",
	"PETER",
	"PETERSBURG",
	"PHILADELPHIA",
	"PHILIP",
	"PHILIPPE",
	"PHOENIX",
	"PICKENS",
	"PICOT",
	"PIERANTONIO",
	"PIERGIORGIO",
	"PIERLUIGI",
	"PIERO",
	"PIERPAOLO",
	"PIERRE",
	"PIETRO",
	"PILAR",
	"PINE",
	"PINK",
	"PIOTR",
	"PISA",
	"PLAMEN",
	"PLANO",
	"PLEASANT",
	"POCOS",
	"POINT",
	"POLICHRONIS",
	"PONCA",
	"PORET",
	"PORT",
	"PORTLAND",
	"PORTSMOUTH",
	"PORUM",
	"POSSESSION",
	"POTOMAC",
	"POULSEN",
	"POWELL",
	"PRAIA",
	"PRAIRIE",
	"PRIMITIVO",
	"PRIMO",
	"PRINCETON",
	"PROBST",
	"PROMETHEUS",
	"PUYO",
	"PYEONG",
	"QUINTUS",
	"QUIQUE",
	"RADEK",
	"RADIO",
	"RADIOCLUB",
	"RAFA",
	"RAFAEL",
	"RALEIGH",
	"RAMIRO",
	"RAMON",
	"RANIERI",
	"RAOUL",
	"RAPIDS",
	"RAT",
	"RATON",
	"RAUL",
	"RAY",
	"RAYMORE",
	"READING",
	"RED",
	"REG",
	"REINE",
	"REMIGIUSZ",
	"REMO",
	"REMZI",
	"RENATO",
	"RENE",
	"RENO",
	"RHOME",
	"RICARDO",
	"RICCARDO",
	"RICHARD",
	"RICK",
	"RIDGE",
	"RIGHINI",
	"RINO",
	"RIO",
	"RISPOLI",
	"RIVER",
	"RIZZO",
	"ROBERT",
	"ROBERTO",
	"ROBIN",
	"ROCCO",
	"ROCHELLE",
	"ROCHESTER",
	"ROCIO",
	"ROCK",
	"ROCKY",
	"ROD",
	"RODERICK",
	"RODOLFO",
	"RODRIGUE",
	"ROGELIO",
	"ROGER",
	"ROGERSVILLE",
	"ROLF",
	"ROMA",
	"ROMAN",
	"ROMANO",
	"ROMARIC",
	"ROMEO",
	"RON",
	"RONALD",
	"RONG",
	"RONNIE",
	"ROSA",
	"ROSARIO",
	"ROSOLINO",
	"ROTTERDAM",
	"ROVERETO",
	"ROY",
	"RUBEN",
	"RUTI",
	"RYSZARD",
	"SABATER",
	"SABET",
	"SAFAK",
	"SAINT",
	"SALEMI",
	"SALVADO",
	"SALVADOR",
	"SALVATORE",
	"SAMUELE",
	"SAN",
	"SANCHEZ",
	"SANDRO",
	"SANOK",
	"SANTA",
	"SANTE",
	"SANTIAGO",
	"SANTO",
	"SANTOS",
	"SAO",
	"SARASOTA",
	"SASKATOON",
	"SASTAMALA",
	"SAURO",
	"SAVANNH",
	"SAVYTSKYI",
	"SCHMID",
	"SCHOERG",
	"SCOTTSBURG",
	"SCUOLA",
	"SEABROOK",
	"SEAFORD",
	"SEAN",
	"SEBASTIAN",
	"SEBASTIANO",
	"SECONDARIA",
	"SECONDO",
	"SEDAT",
	"SEOUL",
	"SERAFIN",
	"SERENA",
	"SERGIO",
	"SERGIPE",
	"SERKAN",
	"SERTAC",
	"SETAUKET",
	"SETTLE",
	"SEVERINO",
	"SEYMOUR",
	"SH",
	"SHAH",
	"SHAKOPEE",
	"SHANE",
	"SHAWNEE",
	"SHEFFIELD",
	"SHENZHEN",
	"SHERBROOKE",
	"SHILOH",
	"SHIRLEY",
	"SILENT",
	"SILVANO",
	"SILVIO",
	"SIMON",
	"SIMPLICIO",
	"SIMSBURY",
	"SLU",
	"SNEADS",
	"SNORRE",
	"SOERENSEN",
	"SOFIANOS",
	"SOMERS",
	"SONOMA",
	"SORBOLO",
	"SOUTH",
	"SOUTHGATE",
	"SP",
	"SPARKS",
	"SPEZIA",
	"SPIROS",
	"SPORS",
	"SPRINGFIELD",
	"SPRINGS",
	"SPYROS",
	"SQUARE",
	"ST",
	"STAFFANSTORP",
	"STAMATIS",
	"STATION",
	"STAVROS",
	"STEFAN",
	"STEFANO",
	"STEPHANE",
	"STEPHEN",
	"STEVANI",
	"STEVE",
	"STEVEN",
	"STEWART",
	"STIIG",
	"STORTFORD",
	"STRATFORD",
	"STUART",
	"SUL",
	"SUMMERVILLE",
	"SUNDSBRUK",
	"SUNDSVALL",
	"SURFSIDE",
	"SUSAN",
	"SUSO",
	"SUWON",
	"SVARTSJOE",
	"SVENDBORG",
	"SWANSEA",
	"SYDNEY",
	"SZCZECIN",
	"SZOKE",
	"TAGLIACOZZO",
	"TAIPEI",
	"TALLASEN",
	"TAMAROA",
	"TAMER",
	"TAMPERE",
	"TAURANGA",
	"TED",
	"TEKIN",
	"TEMPE",
	"TEMPERANCE",
	"TEODORO",
	"TERENCE",
	"TERNI",
	"TERRY",
	"TERVO",
	"TESTE",
	"THEODOROS",
	"THERESE",
	"THIELLLS",
	"THIERRY",
	"THOMANN",
	"THOMAS",
	"TILICI",
	"TIM",
	"TIMOTHY",
	"TITO",
	"TITUSVILLE",
	"TIZAPAN",
	"TOLGA",
	"TOM",
	"TOMAS",
	"TOMASZ",
	"TOMMASO",
	"TOMS",
	"TONAMI",
	"TONI",
	"TONINO",
	"TONY",
	"TOP",
	"TORE",
	"TORI",
	"TORINO",
	"TORNIO",
	"TORONTO",
	"TOWNSHIP",
	"TRAIL",
	"TREMONT",
	"TREMOSNA",
	"TREVOR",
	"TRIANTAFYLLOS",
	"TRISTAN",
	"TRUGLIA",
	"TRUMBULL",
	"TRUQUET",
	"TUCSON",
	"TULLIO",
	"TULSA",
	"TURITTO",
	"TWP",
	"TYRESO",
	"ULSAN",
	"UMBERTO",
	"UNIDAD",
	"UPPSALA",
	"URCE",
	"URE",
	"URLE",
	"URPLACOM",
	"USER",
	"VA",
	"VALAIS",
	"VALENCIA",
	"VALENTIN",
	"VALENTINO",
	"VALERIO",
	"VALLAMAND",
	"VALLSTA",
	"VANCOUVER",
	"VANIER",
	"VANJA",
	"VANTAA",
	"VARENNES",
	"VARGARDA",
	"VASILEIOS",
	"VASILIS",
	"VAXHOLM",
	"VEDAT",
	"VEGAS",
	"VENICE",
	"VENTIMIGLIA",
	"VERON",
	"VERONA",
	"VESSKO",
	"VHF",
	"VIC",
	"VICENTE",
	"VICTOR",
	"VINCENZO",
	"VIRGILIO",
	"VIRNA",
	"VISALIA",
	"VISTA",
	"VITO",
	"VITTORIO",
	"VLADIMIR",
	"VLADISLAVS",
	"VOLKAN",
	"VOLTERRA",
	"VORDINGBORG",
	"VUITEL",
	"WALLINGFORD",
	"WALNUT",
	"WALTER",
	"WAPPINGER",
	"WARREN",
	"WARSAW",
	"WARSZAWA",
	"WASHINGTON",
	"WATERTOWN",
	"WATERVLIET",
	"WAUKESHA",
	"WAYCASSY",
	"WAYNE",
	"WEBB",
	"WEST",
	"WESTBURY",
	"WESTON",
	"WHITE",
	"WHITESTONE",
	"WILKESBORO",
	"WILLIAM",
	"WILLIAMSTOWN",
	"WINDHAM",
	"WINDSOR",
	"WINTER",
	"WINTERPORT",
	"WLADIMIRO",
	"WLODZIMIERZ",
	"WOJCIECH",
	"WOODBRIDGE",
	"WOODLAND",
	"WOODSTOCK",
	"WYEE",
	"WYLIE",
	"XABIER",
	"XAVIER",
	"YANG",
	"YANNICK",
	"YAPHANK",
	"YONKERS",
	"YUMA",
	"YURECUARO",
	"YVAN",
	"YVES",
	"ZARAGOZA",
	"ZBIGNIEW",
	"ZCARLOS",
	"ZDENKO",
	"ZDRAVKO",
	"ZE",
	"ZEVIO",
	"ZH",
]

upperCaseWords = [
	"AA",
	"AB",
	"ACCART",
	"ACRAM",
	"ACT",
	"ACU",
	"AFTT",
	"AFVCR",
	"AFZ",
	"AIRE",
	"AJT",
	"AK",
	"AL",
	"ALA",
	"AMRS",
	"AN",
	"ANFR",
	"APRS",
	"AQ",
	"AR",
	"ARAC",
	"ARACA",
	"AREX",
	"ARG",
	"ARRL",
	"ARRS",
	"ARS",
	"ARSGM",
	"AS",
	"ASORL",
	"ASRR",
	"ASTT",
	"AT",
	"ATL",
	"AU",
	"AUS",
	"AWR",
	"AXPE",
	"AZ",
	"BA",
	"BB",
	"BC",
	"BE",
	"BG",
	"BGD",
	"BK",
	"BL",
	"BR",
	"BRU",
	"BSG",
	"BW",
	"BXE",
	"BZ",
	"CA",
	"CAN",
	"CARG",
	"CAS",
	"CDARC",
	"CH",
	"CIS",
	"CK",
	"CKRG",
	"CL",
	"CM",
	"CMARS",
	"CN",
	"CR",
	"CRD",
	"CT",
	"CY",
	"CZ",
	"DARC",
	"DARES",
	"DC",
	"DEU",
	"DFMA",
	"DFW",
	"DK",
	"DLR",
	"DLZA",
	"DM",
	"DMR",
	"DOK",
	"DR",
	"DRC",
	"DSTAR",
	"DSTAT",
	"DT",
	"EC",
	"EDF",
	"EDR",
	"EE",
	"ELDA",
	"ELZAS",
	"EMA",
	"EMC",
	"EMRE",
	"ERA",
	"ES",
	"FBI",
	"FD",
	"FFBG",
	"FI",
	"FM",
	"FMLOG",
	"FR",
	"FRA",
	"FRAG",
	"FRAT",
	"FRO",
	"FUAT",
	"GA",
	"GBHARC",
	"GBN",
	"GE",
	"GFB",
	"GM",
	"GP",
	"GR",
	"GT",
	"GV",
	"GW",
	"HB",
	"HE",
	"HFP",
	"HH",
	"HI",
	"HK",
	"HKG",
	"HKI",
	"HOLO",
	"HORK",
	"HP",
	"HR",
	"HRO",
	"HRS",
	"HS",
	"HT",
	"HU",
	"HV",
	"IA",
	"ICOM",
	"ID",
	"IE",
	"IF",
	"IG",
	"IGBF",
	"II",
	"III",
	"IL",
	"IN",
	"IPHA",
	"IPR",
	"IT",
	"IV",
	"JAM",
	"JB",
	"JEZ",
	"JF",
	"JH",
	"JL",
	"JM",
	"JMARC",
	"JOTA",
	"KBH",
	"KLN",
	"KP",
	"KPGK",
	"KR",
	"KS",
	"KTK",
	"KW",
	"KY",
	"LARU",
	"LI",
	"LLV",
	"LRRA",
	"LU",
	"LV",
	"MA",
	"MAD",
	"MB",
	"MBG",
	"MD",
	"ME",
	"MERT",
	"MI",
	"MK",
	"MLARS",
	"MLB",
	"MM",
	"MN",
	"MO",
	"MOKPO",
	"MS",
	"MT",
	"MV",
	"MX",
	"MY",
	"NASA",
	"NB",
	"NC",
	"NCD",
	"ND",
	"NDR",
	"NE",
	"NH",
	"NJ",
	"NJECT",
	"NL",
	"NM",
	"NOZ",
	"NRRL",
	"NS",
	"NT",
	"NV",
	"NW",
	"NY",
	"NYC",
	"NZ",
	"OE",
	"OEARC",
	"OEVSV",
	"OK",
	"OR",
	"OT",
	"OV",
	"PA",
	"PBL",
	"PCRE",
	"PE",
	"PH",
	"PL",
	"PR",
	"PT",
	"PY",
	"PZK",
	"QA",
	"QC",
	"QLD",
	"RA",
	"RAAI",
	"RACCW",
	"RACW",
	"RAGLAN",
	"RATS",
	"RC",
	"RCA",
	"RCL",
	"RCV",
	"RE",
	"REC",
	"RECEP",
	"REEC",
	"REM",
	"RI",
	"RN",
	"RO",
	"RP",
	"RS",
	"RSF",
	"RSGB",
	"RST",
	"RU",
	"RWEG",
	"RWTH",
	"SA",
	"SAR",
	"SAS",
	"SBARC",
	"SC",
	"SCH",
	"SCS",
	"SD",
	"SE",
	"SES",
	"SG",
	"SI",
	"SJ",
	"SK",
	"SL",
	"SM",
	"SNARS",
	"SNCF",
	"SORE",
	"SSA",
	"SSRA",
	"START",
	"SV",
	"SW",
	"SZ",
	"SZCZEPAN",
	"TAA",
	"TAS",
	"TC",
	"TDOTA",
	"TG",
	"TH",
	"THW",
	"TJ",
	"TN",
	"TR",
	"TT",
	"TV",
	"TW",
	"TX",
	"UA",
	"UAICF",
	"UBA",
	"UK",
	"UR",
	"URC",
	"URCAT",
	"URO",
	"URP",
	"US",
	"USKA",
	"UT",
	"UTR",
	"UY",
	"VAN",
	"VBR",
	"VCDRC",
	"VD",
	"VE",
	"VLI",
	"VT",
	"VWV",
	"WA",
	"WAU",
	"WG",
	"WHT",
	"WI",
	"WIMO",
	"WLBR",
	"WLX",
	"WMY",
	"WNA",
	"WV",
	"WX",
	"YOTA",
	"YT",
	"ZA",
]

def upperFirst(x):
    return x[0].upper() + x[1:]

def lowerFirst(x):
    return x[0].lower() + x[1:]

enable_options = [upperFirst(x["name"]) for x in sorted(optionList)]
disable_options = ["No" + x for x in enable_options]

titleCaseDict = {word : word.title() for word in titleCaseWords}

stateAbbrevs = {}
stateAbbrevsInverse = {}
countryAbbrevsInverse = {}

varbose = False
options = {}
files = []
verbatim = []
excludedIDRanges = {}
includedIDRanges = {}
excludedCountries = {}
includedCountries = {}
errors = []

users = {}

def cleanup_blanks(field):
	# remove leading and trailing blanks
	field = field.strip()

	# squeeze repeated blanks into a single blank
	while "  " in field:
		field = field.replace("  ", " ")

	return field

# Remove duplicated surnames
def removeDupSurnames(field):
	words = field.split()
	if len(words) > 2 and words[-2] == words[-1]:
		field = " ".join(words[:-1])

	return field

# if the entire field is repeated, eliminate the redundant repitition
def removeRepeats(field):
	words = field.split()
	if len(words) < 4 or len(words) % 2 != 0:
		return field

	hlen = len(words) / 2
	for i in range(hlen):
		if words[i] != words[i+hlen]:
			return field

	return " ".join(words[:hlen])

# If a word is all caps, only capitalize the first letter
def titleCase(field):
	words = field.split()
	for i, word in enumerate(words):
		if word in titleCaseDict:
			words[i] = titleCaseDict[word]

	return " ".join(words)

# Abbreviate the cardinal directions
def abbrevDirections(field):
	words = field.split(" ")

	abbrev = directionAbbrevs.get(words[0], "")
	if abbrev != "":
		words[0] = abbrev

	return " ".join(words)

def removeSubstr(field, subfield):
	index = field.upper().find(subfield.upper())
	if index >= 0:
		field = field[:index] + field[index+len(subfield):]

	return field

def fixRomanNumerals(field):
	if len(field) < 3:
		return field

	if field[-1] == "i":
		if field.endswith(" Ii"):
			field = field[:-1] + "I"
		elif field.endswith(" Iii"):
			field = field[:-2] + "II"
	elif field[-1] == "v":
		if field.endswith(" Iv"):
			field = field[:-1] + "V"

	return field

# Return True for United States call signs.
def US_Call(user):
	first = user["call"][0]
	second = user["call"][1]
	if first in "KNW":
		return True

	if first == "A" and second >= "A" and second <= "L":
		return True

	return False

def fixStateCountries(user):
	for country, abbrevStates in stateAbbrevsByCountry.iteritems():
		for state in abbrevStates:
			if user["country"] == state:
				if state == "Georgia" and not US_Call(user):
					continue
				if user["state"] == "":
					user["state"] = state
				user["country"] = country
	return user

def checkTitleCase():
	upperCaseDict = {word : True for word in upperCaseWords}
	newWordDict = {}

	for _, user in users.iteritems():
		types = [ "name", "city", "state", "nick", "country"]
		for field in [user[t] for t in types]:
			for word in field.split():
				if len(word) < 2:
					continue

				allUpper = True
				for char in word:
					if char < "A" or char > "Z":
						allUpper = False
						break

				if not allUpper:
					continue

				if word in titleCaseDict:
					continue

				if word in upperCaseDict:
					continue

				newWordDict[word] = True

	if len(newWordDict) == 0:
		print("No new upper-case words.", file=sys.stderr)
		return

	print("New upper-case words:", file=sys.stderr)
	for word in sorted(newWordDict):
		print("\t" + word, file=sys.stderr)

def massage_users():
	for dmr_id, user in users.iteritems():
		# remove blanks from within callsigns for ids >= 1000000
		if int(dmr_id) >= 1000000:
			user["call"] = user["call"].replace(" ", "")

		if options["removeDupSurnames"]:
			user["name"] = removeDupSurnames(user["name"])

		if options["removeRepeats"]:
			for key, val in user.iteritems():
				user[key] = removeRepeats(val)

		if options["titleCase"]:
			user["name"] = titleCase(user["name"])
			user["city"] = titleCase(user["city"])
			#user["state"] = titleCase(user["state"])
			user["nick"] = titleCase(user["nick"])
			#user["country"] = titleCase(user["country"])

		if options["removeMatchingNick"]:
			first = user["name"].split(" ", 2)[0]
			if first == user["nick"]:
				user["nick"] = ""
		else:
			if user["nick"] == "":
				user["nick"] = user["name"].split(" ", 2)[0]

		if options["removeNames"]:
			user["name"] = ""
			user["nick"] = ""

		if options["fixStateCountries"]:
			user = fixStateCountries(user)

		if options["abbrevCountries"]:
			abbrev = countryAbbrevs.get(user["country"].upper(), "")
			if abbrev != "":
				user["country"] = abbrev
		else:
			country = countryAbbrevsInverse.get(user["country"].upper(), "")
			if country != "":
				user["country"] = country

		if options["abbrevStates"]:
			abbrev = stateAbbrevs.get(user["state"].upper(), "")
			if abbrev != "":
				user["state"] = abbrev
		else:
			state = stateAbbrevsInverse.get(user["state"].upper(), "")
			if state != "":
				user["state"] = state

		if options["abbrevDirections"]:
			user["city"] = abbrevDirections(user["city"])
			user["state"] = abbrevDirections(user["state"])

		if options["removeCallFromNick"]:
			user["nick"] = removeSubstr(user["nick"], user["call"])

		if options["miscChanges"]:
			if user["city"].endswith(" (B,"):
				user["city"] = user["city"][:-len(" (B")]

		if options["fixRomanNumerals"]:
			user["name"] = fixRomanNumerals(user["name"])

		for key, val in user.iteritems():
			user[key] = cleanup_blanks(val)

		users[dmr_id] = user

def read_user_line(file, i, line):
	if i == 1 and "," not in line:
		try:
			int(line)
			return
		except ValueError:
			pass

	line = line.strip("\n")

	if line == "":
		print("{0}:{1} Empty line.".format(file.name, i),
			file=sys.stderr)
		return

	fields = line.split(",")

	try:
		i_dmr_id = int(fields[0])
		if i_dmr_id > 16777215:
			print("{0}:{1} Invalid DMR ID value: {2}".format(
				file.name, i, line), file=sys.stderr)
			return
	except ValueError:
		print("{0}:{1} Non-numeric first value (DMR ID): {2}".format(
			file.name, i, line), file=sys.stderr)
		return

	if len(fields) != 7:
		if len(fields) < 7:
			err = "{0}:{1} Too few values ({2}): {3}".format(
				file.name, i, len(fields), line)
			fields += ["", "", "", "", "", "", ""]
		else:
			err = "{0}:{1} Too many values ({2}): {3}".format(
				file.name, i, len(fields), line)

		fields = fields[:7]
		print(err, file=sys.stderr)

	dmr_id, call, name, city, state, nick, country = fields

	if file.name in excludedCountries:
		if country.upper() in excludedCountries[file.name]:
			return

	if "*" in excludedCountries:
		if country.upper() in excludedCountries["*"]:
			return

	includedCountry = True
	if file.name in includedCountries or "*" in includedCountries:
		includedCountry = False
		if file.name in includedCountries:
			if country.upper() in includedCountries[file.name]:
				includedCountry = True
		if "*" in includedCountries:
			if country.upper() in includedCountries["*"]:
				includedCountry = True
	if not includedCountry:
		return

	excludedID = False
	if file.name in excludedIDRanges:
		for idRange in excludedIDRanges[file.name]:
			if i_dmr_id >= idRange[0] and i_dmr_id <= idRange[1]:
				excludedID = True
	if "*" in excludedIDRanges:
		for idRange in excludedIDRanges["*"]:
			if i_dmr_id >= idRange[0] and i_dmr_id <= idRange[1]:
				excludedID = True
	if excludedID:
		return

	includedID = True
	if file.name in includedIDRanges or "*" in includedIDRanges:
		includedID = False
		if file.name in includedIDRanges:
			for idRange in includedIDRanges[file.name]:
				if i_dmr_id >= idRange[0] and i_dmr_id <= idRange[1]:
					includedID = True
		if "*" in includedIDRanges:
			for idRange in includedIDRanges[file.name]:
				if i_dmr_id >= idRange[0] and i_dmr_id <= idRange[1]:
					includedID = True
	if not includedID:
		return

	new_user = {
		"call": call,
		"name": name,
		"city": city,
		"state": state,
		"nick": nick,
		"country": country,
	}

	blank_user = {
		"id": dmr_id,
		"call": "",
		"name": "",
		"city": "",
		"state": "",
		"nick": "",
		"country": "",
	}

	user = users.get(dmr_id, blank_user)

	changed = False
	for key, val in new_user.iteritems():
		if val != "":
			user[key] = val
			changed = True

	if changed:
		users[dmr_id] = user

def excludeIDRanges(filename, idRanges, errPrefix):
	if filename != "*":
		try:
			file = open(filename, "r")
			file.close()
		except IOError as err:
			errors.append(errPrefix + str(err))
			return

	for idRange in idRanges:
		ids = idRange.split("-", 2)
		if len(ids) == 1:
			ids = [ids[0], ids[0]]

		try:
			ids = [int(ids[0]), int(ids[1])]
		except ValueError:
			errors.append(errPrefix + "bad IDRange {0}".format(idRange))
			return

		x = excludedIDRanges.get(filename, None)
		if x == None:
			excludedIDRanges[filename] = []

		excludedIDRanges[filename].append(ids)

def includeIDRanges(filename, idRanges, errPrefix):
	if filename != "*":
		try:
			file = open(filename, "r")
			file.close()
		except IOError as err:
			errors.append(errPrefix + str(err))
			return

	for idRange in idRanges:
		ids = idRange.split("-", 2)
		if len(ids) == 1:
			ids = [ids[0], ids[0]]

		try:
			ids = [int(ids[0]), int(ids[1])]
		except ValueError:
			errors.append(errPrefix + "bad IDRange {0}".format(idRange))
			return

		x = includedIDRanges.get(filename, None)
		if x == None:
			includedIDRanges[filename] = []

		includedIDRanges[filename].append(ids)

def excludeCountries(filename, countries, errPrefix):
	if filename != "*":
		try:
			file = open(filename, "r")
			file.close()
		except IOError as err:
			errors.append(errPrefix + str(err))
			return

	countryMap = {}
	for country in countries:
		country = country.upper()
		countryMap[country] = True
		abbrev = countryAbbrevs.get(country, "")
		if abbrev != "":
			countryMap[abbrev.upper()] = True
		abbrev = countryAbbrevsInverse.get(country, "")
		if abbrev != "":
			countryMap[abbrev.upper()] = True

	x = excludedCountries.get(filename, None)
	if x == None:
		excludedCountries[filename] = []

	excludedCountries[filename].extend(countryMap.keys())

def includeCountries(filename, countries, errPrefix):
	if filename != "*":
		try:
			file = open(filename, "r")
			file.close()
		except IOError as err:
			errors.append(errPrefix + str(err))
			return

	countryMap = {}
	for country in countries:
		country = country.upper()
		countryMap[country] = True
		abbrev = countryAbbrevs.get(country, "")
		if abbrev != "":
			countryMap[abbrev.upper()] = True
		abbrev = countryAbbrevsInverse.get(country, "")
		if abbrev != "":
			countryMap[abbrev.upper()] = True

	x = includedCountries.get(filename, None)
	if x == None:
		includedCountries[filename] = []

	includedCountries[filename].extend(countryMap.keys())

def parseConfigLine(configFile, i, line):
	global verbatim, debug, verbose

	errPrefix = "{0}:{1}: ".format(configFile.name, i)
	tokens = shlex.split(line, True)
	if len(tokens) == 0:
		return

	cmd = tokens[0].lower()
	args = tokens[1:]

	if len(args) == 0:
		for opt in enable_options:
			if opt.lower() == cmd:
				options[opt] = True
				return

		for opt in disable_options:
			if opt.lower() == cmd:
				options[opt[2:]] = False
				return

		if cmd == "debug":
			debug = True
			return

		if cmd == "verbose":
			verbose = True
			return

	if cmd == "file" or cmd == "files":
		for arg in args:
			try:
				file = open(arg, "r")
				files.append(file)
			except IOError as err:
				errors.append(errPrefix + str(err))
		return

	if cmd == "verbatim" or cmd == "verbatims":
		for arg in args:
			try:
				verbatimFile = open(arg, "r")
				verbatim.append(verbatimFile)
			except IOError as err:
				errors.append(errPrefix + str(err))
		return

	if cmd == "excludeid" or cmd == "excludeids":
		filename = args[0]
		idRanges = args[1:]
		excludeIDRanges(filename, idRanges, errPrefix)
		return

	if cmd == "includeid" or cmd == "includeids":
		filename = args[0]
		idRanges = args[1:]
		includeIDRanges(filename, idRanges, errPrefix)
		return

	if cmd == "excludecountry" or cmd == "excludecountries":
		filename = args[0]
		countries = args[1:]
		excludeCountries(filename, countries, errPrefix)
		return

	if cmd == "includecountry" or cmd == "includecountries":
		filename = args[0]
		countries = args[1:]
		includeCountries(filename, countries, errPrefix)
		return

	errors.append(errPrefix + "syntax error")

def process_config_file(configFile):
	i = 1
	for line in configFile:
		parseConfigLine(configFile, i, line)
		i += 1

def process_args():
	global verbatim, debug, verbose

	parser = argparse.ArgumentParser(description="Merge userdb files")

	for opt in optionList:
		parser.add_argument("--" + opt["name"],
			action="store_true", dest=opt["name"], default=None,
			help=opt["help"])
		parser.add_argument("--no" + upperFirst(opt["name"]),
			action="store_false", dest=opt["name"], default=None,
			help="Do not " + lowerFirst(opt["help"]))

	parser.add_argument("--config", nargs=1, metavar="configfilename",
		action="store", type=argparse.FileType("r"),
		help="A file containing configuration flags and options")

	parser.add_argument("--verbatim", "--verbatims", nargs="+",
		metavar="filename", action="append",
		type=argparse.FileType("r"),
		help="A filename whose fields are merged without " +
			"modification. These files are merged after field " +
			"fixups have been applied.")

	parser.add_argument("files", metavar="filename", nargs="*",
		type=argparse.FileType("r"), help="A filename to be merged. " +
		"Files are merged in the order that they are named in this " +
		"list. Fields from later files take precedence over " +
		"(replace) those of previous files.")

	parser.add_argument("-v", "--verbose", help="Enable verbose output.",
		action="store_true")

	parser.add_argument("--debug", help="Enable debugging output.",
		action="store_true")

	parser.add_argument("--version", help="Output the current version.",
		action="store_true")

	parser.add_argument("--excludeID", "--excludeIDs", nargs="+",
		action="append", metavar=("filename", "id1[-id2]"),
		help="This option only applies to the named file. " +
			"Any records with a dmr-id matching id1 or within " +
			"the range id1-id2 is excluded from the output")

	parser.add_argument("--includeID", "--includeIDs", nargs="+",
		action="append", metavar=("filename", "id1[-id2]"),
		help="This option only applies to the named file. " +
			"Only records with a dmr-id matching id1 or within " +
			"the range id1-id2 are included in the output")

	parser.add_argument("--excludeCountry", "--excludeCountries",
		nargs="+", action="append", metavar=("filename", "countryname"),
		help="This option only applies to the named file. " +
			"Any records with a country name matching " +
			"countryname are excluded from the output")

	parser.add_argument("--includeCountry", "--includeCountries",
		nargs="+", action="append", metavar=("filename", "countryname"),
		help="This option only applies to the named file. " +
			"Only records with a country name matching " +
			"countryname are included in the output")

	parser.add_argument("-o", nargs=1, dest="options",
		action="append", choices=enable_options + disable_options,
		help=argparse.SUPPRESS)

	args = parser.parse_args()

	debug = False
	if args.debug:
		debug = True

	verbose = False
	if args.verbose or args.debug:
		verbose = True

	if args.version:
		print(version)
		sys.exit(0)

	for _, abbrevStates in stateAbbrevsByCountry.iteritems():
		stateAbbrevs.update(abbrevStates)

	for state, abbrev in stateAbbrevs.items():
		stateAbbrevs[state.upper()] = abbrev
		existing = stateAbbrevsInverse.get(abbrev.upper(), "")
		if existing != "":
			print("Error: duplicate abbreviation:", abbrev,
				file=sys.stderr)
		stateAbbrevsInverse[abbrev.upper()] = state

	for _, abbrevStates in alternateStateAbbrevsByCountry.iteritems():
		for state, abbrev in abbrevStates.items():
			stateAbbrevs[state.upper()] = abbrev

	for country, abbrev in countryAbbrevs.items():
		countryAbbrevs[country.upper()] = abbrev
		existing = countryAbbrevsInverse.get(abbrev.upper(), "")
		if existing != "":
			print("Error: duplicate abbreviation:", abbrev,
				file=sys.stderr)
		countryAbbrevsInverse[abbrev.upper()] = country

	for country, abbrev in alternateCountryAbbrevs.items():
		countryAbbrevs[country.upper()] = abbrev

	for abbrev, country in inverseCountryAbbrevs.items():
		countryAbbrevsInverse[abbrev.upper()] = country

	for opt in optionList:
		options[opt["name"]] = opt["default"]

	if args.config != None:
		for configFile in args.config:
			process_config_file(configFile)

	for opt in optionList:
		name = opt["name"]
		if vars(args)[name] != None:
			options[name] = vars(args)[name]

	if args.options != None:
		flatOpts = [lowerFirst(opt)
				for sublist in args.options for opt in sublist]
		for opt in optionList:
			name = lowerFirst(opt["name"])
			if name in flatOpts:
				options[name] = True
				if verbose:
					print("'-o " + upperFirst(name) +
						"' is obsolete. Use '--" +
						name + "'.", file=sys.stderr)
				continue
			noname = "no" + upperFirst(name)
			if noname in flatOpts:
				options[lowerFirst(name)] = False
				if verbose:
					print("'-o " + upperFirst(noname) +
						"' is obsolete. Use '--" +
						noname +"'.", file=sys.stderr)

	if args.verbatim != None:
		for verbatimFiles in args.verbatim:
			verbatim += verbatimFiles

	errPrefix = ""

	if args.excludeID != None:
		filename = args.excludeID[0][0]
		idRanges = args.excludeID[0][1:]
		excludeIDRanges(filename, idRanges, errPrefix)

	if args.includeID != None:
		filename = args.includeID[0][0]
		includeIDRanges(filename, idRanges, errPrefix)

	if args.excludeCountry != None:
		filename = args.excludeCountry[0][0]
		countries = args.excludeCountry[0][1:]
		excludeCountries(filename, countries, errPrefix)

	if args.includeCountry != None:
		filename = args.includeCountry[0][0]
		countries = args.includeCountry[0][1:]
		includeCountries(filename, countries, errPrefix)

	files.extend(args.files)

	if len(errors) > 0:
		for error in errors:
			print(error, file=sys.stderr)
		sys.exit(1)

	if verbose:
		for opt in sorted(options):
			print(opt, options[opt], file=sys.stderr)

		for filename in excludedIDRanges:
			for range in excludedIDRanges[filename]:
				if range[0] == range[1]:
					s = str(range[0])
				else:
					s = str(range[0]) + "-" + str(range[1])

				print("excludeID", filename, s, file=sys.stderr)

		for filename in includedIDRanges:
			for range in includedIDRanges[filename]:
				if range[0] == range[1]:
					s = str(range[0])
				else:
					s = str(range[0]) + "-" + str(range[1])

				print("includeID", filename, s, file=sys.stderr)

		for filename in excludedCountries:
			for country in excludedCountries[filename]:
				print("excludeCountry", filename, country,
					file=sys.stderr)

		for filename in includedCountries:
			for country in includedCountries[filename]:
				print("includeCountry", filename, country,
					file=sys.stderr)

		for file in files:
			print("file:", file.name, file=sys.stderr)

		for file in verbatim:
			print("verbatim:", file.name, file=sys.stderr)

def read_user_files(files):
	for file in files:
		i = 1
		for line in file:
			read_user_line(file, i, line)
			i += 1

def output_users():
	byteCount = 0
	lines = [""]
	for i, u in sorted([(int(i), u) for i, u in users.iteritems()]):
		line = "{0},{1},{2},{3},{4},{5},{6}".format(
			u["id"], u["call"], u["name"], u["city"], u["state"],
			u["nick"], u["country"])

		byteCount += len(line) + 1
		lines.append(line)

	lines[0] = str(byteCount)

	if len(lines) == 1 or not options["header"]:
		lines = lines[1:]

	for line in lines:
		print(line)

def main():
	process_args()

	read_user_files(files)
	massage_users()
	read_user_files(verbatim)

	if verbose:
		checkTitleCase()

	output_users()

if __name__ == '__main__':
	main()
