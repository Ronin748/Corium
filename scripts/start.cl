_CORIUM_SCRIPT;

# this script is run when cfg is read

_BEGIN;
	_PRINT "helllll";
	_IF _NRUNNING;
		_RUN "scripts/load_script.cl";
	_ENDIF;
_END;

