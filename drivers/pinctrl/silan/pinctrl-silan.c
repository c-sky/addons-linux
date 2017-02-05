#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/clk.h>
#include <linux/regmap.h>

extern void sc8925_pinctrl_init(void);

static int silan_pinctrl_probe(struct platform_device *pdev)
{
	sc8925_pinctrl_init();
	return 0;
}

static const struct of_device_id silan_pinctrl_dt_match[] = {
	{ .compatible = "silan,sc8925-pinctrl",
		.data = (void *)&sc8925_pinctrl_init },
	{},
};

static struct platform_driver silan_pinctrl_driver = {
	.probe		= silan_pinctrl_probe,
	.driver = {
		.name	= "silan-pinctrl",
		.of_match_table = silan_pinctrl_dt_match,
	},
};

static int __init silan_pinctrl_init(void)
{
	return platform_driver_register(&silan_pinctrl_driver);
}
device_initcall(silan_pinctrl_init);
