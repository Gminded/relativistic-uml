#if ! defined(__RELTIME_H__) && \
	(defined(CONFIG_RELATIVISTIC_TIME) || defined(UML_CONFIG_RELATIVISTIC_TIME))
#define __RELTIME_H__

struct uml_reltime_t {
	long long convergence;
	long double frequency;
};

extern struct uml_reltime_t reltime;

long double atold(char *line);

#endif
