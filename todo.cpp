/*
Bugs :
* Optimierung Überhang-Primer?
* Bericht drucken : lange Zeilen in Vektor-Beschreibung umbrechen
* Transformieren-Dialog ESC

Feature Requests :

Homology Plot
Export plot data
Helical wheel
AS-Sequenz Protease-schneiden
That ligation thingy 

SEE: http://eutils.ncbi.nlm.nih.gov/entrez/query/static/eutils_help.html

----

EMBOSS:
* codon usage statistic
* antigenic prediction
* dotplot
* protease cleavage
* PEST motif prediction
* transmembrane region prediction


----
ALGORITHM for finding siRNA duplexes in mRNA:
for each input sequence:

    find the start position of the CDS in the feature table
    if there is no such CDS, take the -sbegin position as the CDS start

    for each 23 base window along the sequence:

        set the score for this window = 0
        if base 2 of the window is not 'a': ignore this window
        if the window is within 50 bases of the CDS start: ignore this window
	if the window is within 100 bases of the CDS: score = -2
	measure the %GC of the 20 bases from position 2 to 21 of the window
	for the following %GC values change the score:
		%GC <= 25% (<= 5 bases): ignore this window
		%GC 30% (6 bases): score + 0
		%GC 35% (7 bases): score + 2
		%GC 40% (8 bases): score + 4
		%GC 45% (9 bases): score + 5
		%GC 50% (10 bases): score + 6
		%GC 55% (11 bases): score + 5
		%GC 60% (12 bases): score + 4
		%GC 65% (13 bases): score + 2
		%GC 70% (14 bases): score + 0
		%GC >= 75% (>= 15 bases): ignore this window
	if the window starts with a 'AA': score + 3
	if the window does not start 'AA' and it is required: ignore this window
	if the window ends with a 'TT': score + 1
	if the window does not end 'TT' and it is required: ignore this window
	if 4 G's in a row are found: ignore this window
	if any 4 bases in a row are present and not required: ignore this window
	if PolIII probes are required and the window is not NARN(17)YNN: ignore this window
        if the score is > 0: store this window for output
	
    sort the windows found by their score
    output the 23-base windows to the sequence file
    if the 'context' qualifier is specified, output window bases 1 and 2 in brackets to the report file
    take the window bases 3 to 21, add 'dTdT' output to the report file
    take the window bases 3 to 21, reverse complement, add 'dTdT' output to the report file

*/
