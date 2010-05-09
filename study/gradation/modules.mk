mod_gradation.la: mod_gradation.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_gradation.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_gradation.la
