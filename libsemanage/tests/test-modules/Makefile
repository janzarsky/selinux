all: test_bzip_module.cil test_invalid_bzip_module test_hll_module.pp

test_bzip_module.cil: test_bzip_module.src.cil
	bzip2 -c test_bzip_module.src.cil >test_bzip_module.cil

test_invalid_bzip_module: test_bzip_module.src.cil
	bzip2 -c test_bzip_module.src.cil >test_invalid_bzip_module

test_hll_module.pp: test_hll_module.te
	checkmodule -M -m -o test_hll_module.mod test_hll_module.te
	semodule_package -m test_hll_module.mod -o test_hll_module.pp

clean distclean: 
	rm -rf test_bzip_module.cil test_invalid_bzip_module test_hll_module.mod test_hll_module.pp
