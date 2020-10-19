#!/bin/sh

test_rerror ()
{
	test $(wc -l < "$1") -gt 0 || (
		echo "Sortie d'erreur standard vide" >&2
		return 1
	)
	return 0
}

clean_tmp ()
{
	rm -f "tmp*"
}










