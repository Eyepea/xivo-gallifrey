"""ACL and Groups, with potentialy hierarchical arbitrary precedences

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

from copy import deepcopy

UNIVERSE = 'all'

DISALLOWED = -1
NEUTRAL = 0
ALLOWED = 1

def str_credit(x):
	if x == DISALLOWED: return "DISALLOWED"
	if x == NEUTRAL: return "NEUTRAL"
	if x == ALLOWED: return "ALLOWED"
	return "<UNKNOWN>"

class Entity:
	def __init__(self,name,contains,first_allow,allow,disallow,is_group=None,is_res=None):
		self.name = str(name)
		self.contains = tuple(contains)
		self.first_allow = bool(first_allow)
		self.allow = tuple(allow)
		self.disallow = tuple(disallow)
		self.is_group = bool(is_group)
		self.is_res = bool(is_res)

class GroupEntity(Entity):
	def __init__(self,name,contains,first_allow,allow,disallow,is_res=None):
		Entity.__init__(self,name,contains,first_allow,allow,disallow,True,is_res)

class ElemEntity(Entity):
	def __init__(self,name,first_allow,allow,disallow,is_res=None):
		Entity.__init__(self,name,(name,),first_allow,allow,disallow,False,is_res)

class ResEntity(Entity): # warning: quite decorative class
	def __init__(self):
		self.is_res = True

class UserEntity(Entity): # warning: quite decorative class
	def __init__(self):
		self.is_res = False

class UserElemEntity(ElemEntity,UserEntity):
	def __init__(self,name,first_allow,allow,disallow):
		ElemEntity.__init__(self,name,first_allow,allow,disallow)
		UserEntity.__init__(self)

class UserGroupEntity(GroupEntity,UserEntity):
	def __init__(self,name,contains,first_allow,allow,disallow):
		GroupEntity.__init__(self,name,contains,first_allow,allow,disallow)
		UserEntity.__init__(self)

class ResElemEntity(ElemEntity,ResEntity):
	def __init__(self,name,first_allow,allow,disallow):
		ElemEntity.__init__(self,name,first_allow,allow,disallow)
		ResEntity.__init__(self)

class ResGroupEntity(GroupEntity,ResEntity):
	def __init__(self,name,contains,first_allow,allow,disallow):
		GroupEntity.__init__(self,name,contains,first_allow,allow,disallow)
		ResEntity.__init__(self)

def is_in_acl(name,acl):
	return ((UNIVERSE in acl) or (name in acl))

def single_acl_eval(name,first_allow,allow,disallow,dflt):
	if first_allow:
		if is_in_acl(name,disallow):
			return DISALLOWED
		elif is_in_acl(name,allow):
			return ALLOWED
	else:
		if is_in_acl(name,allow):
			return ALLOWED
		elif is_in_acl(name,disallow):
			return DISALLOWED
	return dflt

def check_acl(user_elem,res_elem,grplst_order,dflt_policy,validation_model=DISALLOWED):
	"""Check if a 'user' is allowed to access a 'resource'
	
	* user_elem must have a behavior similar to the one of an UserElemEntity
	instance, and is used to identify the 'user'
	* res_elem must have a behavior similar to the one of an ResElemEntity
	instance, and is used to identify the 'resource'
	* grplst_order must be an iterable object which elements are iterable.
	Elements of grplst_order are lists of access control groups having the
	same priority. grplst_order defines a priority order, first group lists
	having higher precedence than last ones, so they overrides the latter.
	* dflt_policy must be one of DISALLOWED or ALLOWED so it will be used
	if no rule has been found to be applicable to the 'user' / 'resource'
	pair. One can also use NEUTRAL to specifically recognize that no
	matching rule has been found.
	* validation_model must be set to either DISALLOWED, ALLOWED or NEUTRAL.
	When set to DISALLOWED the validation model used is strict, and when
	several contradictory rules of the same precedence are applicable for
	the given 'user' / 'resource' pair, the DISALLOWED result will be
	preferred. When set to ALLOWED, a loose validation model is used so that
	ALLOWED will be preferred in the same situation as the one stated
	before. NEUTRAL has a special meaning: if contradictory rules are found
	while NEUTRAL is in force, the current level of precedence is skipped
	and the search continues at next for non contradictory rules.
	
	Rules evaluations are performed in the following order:
	
		* rules specified inside res_elem or user_elem themselves are
		evaluated first so any specific rules configured at this point
		has the greatest precedence and one can override a more general
		policy by either giving or denying specific permissions for
		individual entities. If no rule match at this point the
		execution continues, in the other case the result is returned
		immediatly.
		
		* grplst_order is iterated over and specify the precedence order
		that must be applied to rules given inside group definitions.
		As grplst_order is indeed a list of list of groups descriptors
		(or something iterable in a two stage compatible way), a given
		set of groups can have the same precedence and when this happen,
		if contradictory rules are found, validation_model will be used
		so the choice will be made at this point between granted,
		denied, or going to the next level to look for coherent rules.
	
	"""
	st = single_acl_eval(res_elem.name,user_elem.first_allow,
			     user_elem.allow,user_elem.disallow,NEUTRAL)
	if validation_model != NEUTRAL and st == validation_model:
		return validation_model
	oldst = st
	st = single_acl_eval(user_elem.name,res_elem.first_allow,
			     res_elem.allow,res_elem.disallow,st)
	if st != NEUTRAL and (validation_model != NEUTRAL
				or oldst == NEUTRAL
				or oldst == st):
		return st
	for grplst in grplst_order:
		st = NEUTRAL
		oldst = NEUTRAL
		for grp in grplst:
			if grp.is_res:
				if is_in_acl(res_elem.name,grp.contains):
					oldst = st
					st = single_acl_eval(
						user_elem.name,
						grp.first_allow,
						grp.allow,
						grp.disallow,
						st)
			else:
				if is_in_acl(user_elem.name,grp.contains):
					oldst = st
					st = single_acl_eval(
						res_elem.name,
						grp.first_allow,
						grp.allow,
						grp.disallow,
						st)
			if validation_model == NEUTRAL and st != NEUTRAL \
			   and oldst != NEUTRAL and st != oldst:
				st = NEUTRAL
				oldst = NEUTRAL
				break
			if validation_model != NEUTRAL and st == validation_model:
				return validation_model
		if st != NEUTRAL:
			return st
	return dflt_policy

def format_loop(prev_dict,grp,v):
	a = [v, grp]
	while prev_dict[v]:
		v = prev_dict[v]
		a.insert(0, v)
	return ' -> '.join(a)

def reachable_elems(group_list,elem_list,conn_grp_elem_dict,impure_function=False):
	"""group_list is a list containing valid group names. It shall not contain
	a group name multiple times.
	
	elem_list is a list containing valid elements names. It shall not
	contain an element name multiple times.
	
	conn_grp_elem_dict is a dictionary of form
		{grp_name_1: ([included_grp_1_name,included_grp_2_name,...],
			      [included_elem_1_name,included_elem_2_name,...]),
		 grp_name_2: ...}
	
	If impure_function is True, the work is done in place in 
	conn_grp_elem_dict
	
	In any case conn_grp_elem_dict must not contain any key not in
	group_list, neither shall it contains anything not in group_list from
	within list stored in element 0 of the tuple dictionary value or
	anything not in elem_list from within list stored in element 1.
	conn_grp_elem_dict must contain an entry for every existing group.
	
	Common errors like loop in the ordered graph will be detected and
	an exception will be raised, but not all error will be detected anyway.
	As far as the internal representation of data passed in parameters is
	compliant with requirements stated above, calling this function should
	be ok.
	
	A dictionary of the same form as conn_grp_elem_dict will be returned,
	except that all lists at position 0 of dictionary values will be empty
	and that all lists at postition 1 will contain every directly or
	indirectly connected to the group which name is defined by the
	corresponding key.
	
	"""
	if impure_function:
		reachable_dict = conn_grp_elem_dict
	else:
		reachable_dict = deepcopy(conn_grp_elem_dict)
	group_set = frozenset(group_list)
	elem_set = frozenset(elem_list)
	for grp in group_list:
		gp_el_lst = []
		gp_el_set = set()
		vertices_done = set()
		vertices_todo_lst = [grp]
		vertices_todo_set = set(vertices_todo_lst)
		prev_dict = {grp: None}
		while vertices_todo_lst:
			v = vertices_todo_lst.pop(0)
			vertices_todo_set.discard(v)
			vertices_done.add(v) # dont loop on ourself even if not current "root"
			if v not in group_set:
				raise ValueError, \
					"Bad destination vertex %s" % v
			for el in reachable_dict[v][1]:
				if el not in elem_set:
					raise ValueError, \
						"Bad element %s within vertex %s" % (el,v)
				if el not in gp_el_set:
					gp_el_lst.append(el)
					gp_el_set.add(el)
			for gp in reachable_dict[v][0]:
				if gp not in group_set:
					raise ValueError, \
						"Bad destination vertex %s from %s" % (gp,v)
				if gp == grp:
					raise ValueError, \
						"Loop detected: %s" % format_loop(prev_dict,grp,v)
				if gp not in vertices_done and gp not in vertices_todo_set:
					prev_dict[gp] = v
					vertices_todo_lst.append(gp)
					vertices_todo_set.add(gp)
		del reachable_dict[grp][0][:]
		del reachable_dict[grp][1][:]
		reachable_dict[grp][1].extend(gp_el_lst)
	return reachable_dict
	

if __debug__:
    if __name__ == "__main__":
	import sys
	attrs = ['allow', 'contains', 'disallow', 'first_allow', 'is_group', 'is_res', 'name']
	x = Entity("a",("a",),False,(),(),False,False)
	xatt = [e for e in dir(x) if '__' not in e]
	assert attrs == xatt, "wrong attributes, test cases need updates"

	get_tuple_vals = lambda x: tuple(map(lambda k:getattr(x,k), attrs))
	test_cases = (
		(ResGroupEntity, ('bla',(),False,(),()),
			((),(),(),False,True,True,"bla")),
		(ResGroupEntity, ('bla',('a',),True,('1','2'),('Z','Y','X')),
			(('1','2'),('a',),('Z','Y','X'),True,True,True,"bla")),
		(UserGroupEntity, ('bla',(),False,(),()),
			((),(),(),False,True,False,"bla")),
		(UserGroupEntity, ('bla',('a',),True,('1','2'),('Z','Y','X')),
			(('1','2'),('a',),('Z','Y','X'),True,True,False,"bla")),

		(ResElemEntity, ('bla',False,(),()),
			((),('bla',),(),False,False,True,"bla")),
		(ResElemEntity, ('bla',True,('1','2'),('Z','Y','X')),
			(('1','2'),('bla',),('Z','Y','X'),True,False,True,"bla")),
		(UserElemEntity, ('bla',False,(),()),
			((),('bla',),(),False,False,False,"bla")),
		(UserElemEntity, ('bla',True,('1','2'),('Z','Y','X')),
			(('1','2'),('bla',),('Z','Y','X'),True,False,False,"bla")),
	)
	for func,args,res in test_cases:
		assert get_tuple_vals(func(*args)) == res, \
			("Test failed for construction of %s with params %s, " +
			 "expected %s got %s") % (func.__name__, str(args),
			 str(res), str(get_tuple_vals(func(*args))))

	def create_group_dico(facto,name,contains,cartname,othercartname):
		if contains is None:
			creator = lambda a,b,c,d,e: facto(a,c,d,e)
		else:
			creator = facto

		d = {}

		d['dn_an']  = creator(name,contains,False,(),())
		d['dn_ao']  = creator(name,contains,False,(othercartname,),())
		d['dn_ar']  = creator(name,contains,False,(cartname,),())
		d['dn_aro'] = creator(name,contains,False,(cartname,othercartname),())
		d['dn_aor'] = creator(name,contains,False,(othercartname,cartname),())
		d['dn_aa']  = creator(name,contains,False,(UNIVERSE,),())

		d['an_dn']  = creator(name,contains,True,(),())
		d['ao_dn']  = creator(name,contains,True,(othercartname,),())
		d['ar_dn']  = creator(name,contains,True,(cartname,),())
		d['aro_dn'] = creator(name,contains,True,(cartname,othercartname),())
		d['aor_dn'] = creator(name,contains,True,(othercartname,cartname),())
		d['aa_dn']  = creator(name,contains,True,(UNIVERSE,),())

		d['da_an']  = creator(name,contains,False,(),(UNIVERSE,))
		d['da_ao']  = creator(name,contains,False,(othercartname,),(UNIVERSE,))
		d['da_ar']  = creator(name,contains,False,(cartname,),(UNIVERSE,))
		d['da_aro'] = creator(name,contains,False,(cartname,othercartname),(UNIVERSE,))
		d['da_aor'] = creator(name,contains,False,(othercartname,cartname),(UNIVERSE,))
		d['da_aa']  = creator(name,contains,False,(UNIVERSE,),(UNIVERSE,))

		d['an_da']  = creator(name,contains,True,(),(UNIVERSE,))
		d['ao_da']  = creator(name,contains,True,(othercartname,),(UNIVERSE,))
		d['ar_da']  = creator(name,contains,True,(cartname,),(UNIVERSE,))
		d['aro_da'] = creator(name,contains,True,(cartname,othercartname),(UNIVERSE,))
		d['aor_da'] = creator(name,contains,True,(othercartname,cartname),(UNIVERSE,))
		d['aa_da']  = creator(name,contains,True,(UNIVERSE,),(UNIVERSE,))

		d['do_an']  = creator(name,contains,False,(),(othercartname,))
		d['do_ar']  = creator(name,contains,False,(cartname,),(othercartname,))
		d['do_aa']  = creator(name,contains,False,(UNIVERSE,),(othercartname,))

		d['dr_an']  = creator(name,contains,False,(),(cartname,))
		d['dr_ar']  = creator(name,contains,False,(cartname,),(cartname,))
		d['dr_aa']  = creator(name,contains,False,(UNIVERSE,),(cartname,))

		d['dor_an'] = creator(name,contains,False,(),(othercartname,cartname))
		d['dor_ar'] = creator(name,contains,False,(cartname,),(othercartname,cartname))
		d['dor_aa'] = creator(name,contains,False,(UNIVERSE,),(othercartname,cartname))

		d['an_do']  = creator(name,contains,True,(),(othercartname,))
		d['ar_do']  = creator(name,contains,True,(cartname,),(othercartname,))
		d['aa_do']  = creator(name,contains,True,(UNIVERSE,),(othercartname,))

		d['an_dr']  = creator(name,contains,True,(),(cartname,))
		d['ar_dr']  = creator(name,contains,True,(cartname,),(cartname,))
		d['aa_dr']  = creator(name,contains,True,(UNIVERSE,),(cartname,))

		d['an_dro'] = creator(name,contains,True,(),(cartname,othercartname))
		d['ar_dro'] = creator(name,contains,True,(cartname,),(cartname,othercartname))
		d['aa_dro'] = creator(name,contains,True,(UNIVERSE,),(cartname,othercartname))

		return d

	def create_elem_dico(facto,name,cartname,othercartname):
		return create_group_dico(facto,name,None,cartname,othercartname)

	ue_dico = create_elem_dico(UserElemEntity, "user", "res", "other_res")
	re_dico = create_elem_dico(ResElemEntity, "res", "user", "other_user")

	matching_user_gpe_dico = create_group_dico(UserGroupEntity, "groupuser", ("user",), "res", "other_res")
	matching_res_gpe_dico  = create_group_dico(ResGroupEntity, "groupres", ("res",), "user", "other_user")

	empty_user_gpe_dico = create_group_dico(UserGroupEntity, "groupuser", (), "res", "other_res")
	empty_res_gpe_dico  = create_group_dico(ResGroupEntity, "groupres", (), "user", "other_user")

	non_match_user_gpe_dico = create_group_dico(UserGroupEntity, "groupuser", ("other_user",), "res", "other_res")
	non_match_res_gpe_dico  = create_group_dico(ResGroupEntity, "groupres", ("other_res",), "user", "other_user")

	late_match_user_gpe_dico = create_group_dico(UserGroupEntity, "groupuser", ("other_user","user"), "res", "other_res")
	late_match_res_gpe_dico  = create_group_dico(ResGroupEntity, "groupres", ("other_res","res"), "user", "other_user")

	allow_list = ('dn_ar','dn_aro','dn_aor','dn_aa',
		      'da_ar','da_aro','da_aor','da_aa',
		      'ar_dn','aro_dn','aor_dn','aa_dn',
		      'do_ar','do_aa','ar_do','aa_do',
		      'dr_ar','dr_aa','dor_ar','dor_aa')
	disallow_list = ('da_an','da_ao','an_da','ao_da',
			 'ar_da','aro_da','aor_da','aa_da',
			 'dr_an','dor_an','an_dr','ar_dr',
			 'aa_dr','an_dro','ar_dro','aa_dro')
	neutral_list = ('dn_an','dn_ao','an_dn','ao_dn','do_an','an_do')

	assert len(allow_list) + len(disallow_list) + len(neutral_list) == len(ue_dico)

	for x in allow_list:
		assert (x not in disallow_list) and (x not in neutral_list)
	for x in disallow_list:
		assert (x not in neutral_list)
		
	def do_tests_atom(grplst_order,expected_at_gpe_level,dflt_pol,val_model):

		for x in allow_list:
			assert check_acl(ue_dico[x],re_dico['dn_an'],grplst_order,dflt_pol,val_model) == ALLOWED,\
				"Failed for x = %s, dflt_pol = %s, val_model = %s" \
				% (x,str_credit(dflt_pol),str_credit(val_model))

		for x in disallow_list:
			assert check_acl(ue_dico[x],re_dico['dn_an'],grplst_order,dflt_pol,val_model) == DISALLOWED,\
				"Failed for x = %s, dflt_pol = %s, val_model = %s" \
				% (x,str_credit(dflt_pol),str_credit(val_model))

		for x in neutral_list:
			for y in neutral_list:
				if expected_at_gpe_level == NEUTRAL:
					iwant = dflt_pol
				else:
					iwant = expected_at_gpe_level
				assert check_acl(ue_dico[x],re_dico[y],grplst_order,dflt_pol,val_model) == iwant,\
					"Failed for x = %s, y = %s, dflt_pol = %s, val_model = %s, iwant = %s" \
					% (x,y,str_credit(dflt_pol),str_credit(val_model),str_credit(iwant))

		for x in allow_list:
			for y in allow_list:
				assert check_acl(ue_dico[x],re_dico[y],grplst_order,dflt_pol,val_model) == ALLOWED,\
					"Failed for x = %s, y = %s, dflt_pol = %s, val_model = %s" \
					% (x,y,str_credit(dflt_pol),str_credit(val_model))

		for x in disallow_list:
			for y in disallow_list:
				assert check_acl(ue_dico[x],re_dico[y],grplst_order,dflt_pol,val_model) == DISALLOWED,\
					"Failed for x = %s, y = %s, dflt_pol = %s, val_model = %s" \
					% (x,y,str_credit(dflt_pol),str_credit(val_model))

		for x in allow_list:
			for y in disallow_list:
				if val_model == NEUTRAL:
					if expected_at_gpe_level != NEUTRAL:
						iwant = expected_at_gpe_level
					else:
						iwant = dflt_pol
				else:
					iwant = val_model
				assert check_acl(ue_dico[x],re_dico[y],grplst_order,dflt_pol,val_model) == iwant,\
					"Failed for x = %s, y = %s, dflt_pol = %s, val_model = %s, iwant = %s" \
					% (x,y,str_credit(dflt_pol),str_credit(val_model),str_credit(iwant))

		for x in disallow_list:
			for y in allow_list:
				if val_model == NEUTRAL:
					if expected_at_gpe_level != NEUTRAL:
						iwant = expected_at_gpe_level
					else:
						iwant = dflt_pol
				else:
					iwant = val_model
				assert check_acl(ue_dico[x],re_dico[y],grplst_order,dflt_pol,val_model) == iwant,\
					"Failed for x = %s, y = %s, dflt_pol = %s, val_model = %s, iwant = %s" \
					% (x,y,str_credit(dflt_pol),str_credit(val_model),str_credit(iwant))

	def do_tests(grplst_order,expected_at_gpe_level):
		for dflt_pol in (NEUTRAL,ALLOWED,DISALLOWED):
			for val_model in (NEUTRAL,ALLOWED,DISALLOWED):
				do_tests_atom(grplst_order,expected_at_gpe_level,dflt_pol,val_model)

	print "Groupless tests"
	do_tests((),NEUTRAL)

	short_allow_list = ('dn_ar','da_aro','aor_dn','aa_do','da_aa')
	for x in short_allow_list:
		assert x in allow_list
	short_disallow_list = ('da_an','aro_da','an_dr','aa_dro')
	for x in short_disallow_list:
		assert x in disallow_list
	short_neutral_list = ('dn_an','an_do')
	for x in short_neutral_list:
		assert x in neutral_list

	print "One group tests"
	for x in short_allow_list + short_disallow_list + neutral_list:

		expect = x in allow_list and ALLOWED or \
			 x in disallow_list and DISALLOWED or \
			 NEUTRAL

		def do_test_report_where_on_except(gpe,strgpe,expect):
			try:
				do_tests(((gpe[x],),),expect)
			except:
				print >> sys.stderr, "for %s[\"%s\"], gpe_expect=%s" % (strgpe,x,str_credit(expect))
				raise

		do_test_report_where_on_except(matching_user_gpe_dico,"matching_user_gpe_dico",expect)
		do_test_report_where_on_except(matching_res_gpe_dico,"matching_res_gpe_dico",expect)
		do_test_report_where_on_except(late_match_user_gpe_dico,"late_match_user_gpe_dico",expect)
		do_test_report_where_on_except(late_match_res_gpe_dico,"late_match_res_gpe_dico",expect)

		do_test_report_where_on_except(empty_user_gpe_dico,"empty_user_gpe_dico",NEUTRAL)
		do_test_report_where_on_except(empty_res_gpe_dico,"empty_res_gpe_dico",NEUTRAL)
		do_test_report_where_on_except(non_match_user_gpe_dico,"non_match_user_gpe_dico",NEUTRAL)
		do_test_report_where_on_except(non_match_res_gpe_dico,"non_match_res_gpe_dico",NEUTRAL)

	short_allow_list = ('dn_ar','da_aro')
	for x in short_allow_list:
		assert x in allow_list
	short_disallow_list = ('da_an','aa_dro')
	for x in short_disallow_list:
		assert x in disallow_list
	short_neutral_list = ('dn_an',)
	for x in short_neutral_list:
		assert x in neutral_list

	def tst_by_group(dflt_pol, potential_fgpe = None):
		for x in short_allow_list + short_disallow_list + neutral_list:
			expect_x = x in allow_list and ALLOWED or \
				   x in disallow_list and DISALLOWED or \
				   NEUTRAL
			for y in short_allow_list + short_disallow_list + neutral_list:
				expect_y = y in allow_list and ALLOWED or \
					   y in disallow_list and DISALLOWED or \
					   NEUTRAL
				def evaluate(e1,e2,vm,dflt):
					if e1 == NEUTRAL and e2 == NEUTRAL: return dflt
					elif e1 == NEUTRAL: return e2
					elif e2 == NEUTRAL: return e1
					elif e1 == e2: return e1
					elif vm == NEUTRAL: return dflt
					else: return vm
				def do_test_report_where_on_except(gpe1,gpe2,strgpe1,strgpe2,e1,e2):
					for val_model in (NEUTRAL,ALLOWED,DISALLOWED):
						expect = evaluate(e1,e2,val_model,dflt_pol)
						try:
							if potential_fgpe is None:
								do_tests_atom(((gpe1[x],gpe2[y]),),expect,dflt_pol,val_model)
							else:
								do_tests_atom(((gpe1[x],gpe2[y]),(potential_fgpe,)),expect,dflt_pol,val_model)
						except:
							print >> sys.stderr, "potential_fgpe = %s" % str(potential_fgpe)
							print >> sys.stderr, "for %s[\"%s\"],%s[\"%s\"] e1=%s e2=%s dflt_pol=%s val_model=%s e=%s" % \
										(strgpe1,x,strgpe2,y,str_credit(e1),str_credit(e2),str_credit(dflt_pol),str_credit(val_model),str_credit(expect))
							raise
				do_test_report_where_on_except(
					matching_user_gpe_dico,matching_res_gpe_dico,
					"matching_user_gpe_dico","matching_res_gpe_dico",
					expect_x,expect_y)
				if potential_fgpe is not None:
					do_test_report_where_on_except(
						matching_user_gpe_dico,empty_user_gpe_dico,
						"matching_user_gpe_dico","empty_user_gpe_dico",
						expect_x,NEUTRAL)
					do_test_report_where_on_except(
						empty_user_gpe_dico,late_match_res_gpe_dico,
						"empty_user_gpe_dico","late_match_res_gpe_dico",
						NEUTRAL,expect_y)
					do_test_report_where_on_except(
						non_match_user_gpe_dico,non_match_res_gpe_dico,
						"non_match_user_gpe_dico","non_match_res_gpe_dico",
						NEUTRAL,NEUTRAL)

	print "Two group tests"
	for dflt_pol in (NEUTRAL,ALLOWED,DISALLOWED):
		tst_by_group(dflt_pol)

	print "One + Two group tests, low prio is not NEUTRAL"
	my_dflt_pol = NEUTRAL
	for t in short_allow_list + short_disallow_list:
		print "testing %s" % t
		if t in allow_list:
			dflt_pol = ALLOWED
		elif t in disallow_list:
			dflt_pol = DISALLOWED
		try:
			tst_by_group(dflt_pol, matching_user_gpe_dico[t])
			tst_by_group(dflt_pol, matching_res_gpe_dico[t])
			tst_by_group(dflt_pol, late_match_user_gpe_dico[t])
			tst_by_group(my_dflt_pol, empty_user_gpe_dico[t])
		except:
			print >> sys.stderr, "t=%s dflt_pol=%s my_dflt_pol=%s" % (t,str_credit(dflt_pol),str_credit(my_dflt_pol))
			raise
	print "One + Two group tests, low prio is NEUTRAL"
	for dflt_pol in (NEUTRAL,ALLOWED,DISALLOWED):
		for t in short_neutral_list:
			try:
				tst_by_group(dflt_pol, matching_user_gpe_dico[t])
				tst_by_group(dflt_pol, empty_user_gpe_dico[t])
			except:
				print >> sys.stderr, "t=%s dflt_pol=%s my_dflt_pol=%s" % (t,str_credit(dflt_pol),str_credit(my_dflt_pol))
				raise

	print "All tests OK"
