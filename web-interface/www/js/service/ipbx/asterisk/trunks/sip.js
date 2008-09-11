var xivo_ast_trunks_elt_default = {
	'protocol-username': {it: true, fd: true},
	'protocol-qualify': {it: true, fd: true},
	'protocol-rtptimeout': {it: true, fd: true},
	'protocol-rtpholdtimeout': {it: true, fd: true},
	'protocol-rtpkeepalive': {it: true, fd: true},
	'protocol-g726nonstandard': {it: true, fd: true},
	'protocol-port': {it: true, fd: true},
	'protocol-usereqphone': {it: true, fd: true},
	'protocol-fromuser': {it: true, fd: true},
	'protocol-fromdomain': {it: true, fd: true},
	'protocol-host-static': {it: true, fd: true},
	'protocol-host-dynamic': {it: true, fd: true}};

var xivo_ast_trunk_type_elt = {};
xivo_ast_trunk_type_elt['peer'] = {};
xivo_ast_trunk_type_elt['friend'] = {};
xivo_ast_trunk_type_elt['user'] = {
	'protocol-username': {it: false, fd: false},
	'protocol-qualify': {it: false, fd: false},
	'protocol-rtptimeout': {it: false, fd: false},
	'protocol-rtpholdtimeout': {it: false, fd: false},
	'protocol-rtpkeepalive': {it: false, fd: false},
	'protocol-g726nonstandard': {it: false, fd: false},
	'protocol-port': {it: false, fd: false},
	'protocol-usereqphone': {it: false, fd: false},
	'protocol-fromuser': {it: false, fd: false},
	'protocol-fromdomain': {it: false, fd: false},
	'protocol-host-static': {it: false, fd: false},
	'protocol-host-dynamic': {it: {property: {disabled: true,
						  className: 'it-readonly',
						  value: 'dynamic'}},
				  fd: true}};
