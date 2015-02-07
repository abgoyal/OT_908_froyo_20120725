/*
 * leds-breath.c - MSM CHARGER LEDs driver.
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>

#include <mach/pmic.h>
#include <mach/mpp.h>
#include <linux/workqueue.h>

static struct delayed_work breath_led_work;

static void msm_breath_led_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	if(value>0)
		mpp_config_digital_out(20, MPP_CFG(MPP_DLOGIC_LVL_VDD, MPP_DLOGIC_OUT_CTRL_LOW));
	else
		mpp_config_digital_out(20, MPP_CFG(MPP_DLOGIC_LVL_VDD, MPP_DLOGIC_OUT_CTRL_HIGH));	
	
	printk(KERN_ERR"breath indicater,state is %s\n",(value>0)?"ON":"OFF");
}

static struct led_classdev msm_breath_led = {
	.name			= "breath-leds",
	.brightness_set		= msm_breath_led_set,
	.brightness		= LED_OFF,
};

static void update_breath_led_state(struct work_struct *work)
{
	msm_breath_led_set(&msm_breath_led, LED_OFF);	
}

static int msm_breath_led_probe(struct platform_device *pdev)
{
	int rc;

	rc = led_classdev_register(&pdev->dev, &msm_breath_led);
	if (rc) {
		dev_err(&pdev->dev, "unable to register led class driver.\n");
		return rc;
	}
	msm_breath_led_set(&msm_breath_led, LED_FULL);
	schedule_delayed_work(&breath_led_work, 3 * HZ);
	return rc;
}

static int __devexit msm_breath_led_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&msm_breath_led);

	return 0;
}

#ifdef CONFIG_PM
static int msm_breath_led_suspend(struct platform_device *dev,
		pm_message_t state)
{
	led_classdev_suspend(&msm_breath_led);
	return 0;
}

static int msm_breath_led_resume(struct platform_device *dev)
{
	led_classdev_resume(&msm_breath_led);
	return 0;
}
#else
#define msm_breath_led_suspend NULL
#define msm_breath_led_resume NULL
#endif

static struct platform_driver msm_breath_led_driver = {
	.probe		= msm_breath_led_probe,
	.remove		= __devexit_p(msm_breath_led_remove),
	.suspend		= msm_breath_led_suspend,
	.resume		= msm_breath_led_resume,
	.driver		= {
		.name	= "breath-leds",
		.owner	= THIS_MODULE,
	},
};

static int __init msm_breath_led_init(void)
{
	printk(KERN_ERR"msm breath led init.\n");
	INIT_DELAYED_WORK(&breath_led_work, update_breath_led_state);
	return platform_driver_register(&msm_breath_led_driver);
}
module_init(msm_breath_led_init);

static void __exit msm_breath_led_exit(void)
{
	platform_driver_unregister(&msm_breath_led_driver);
}
module_exit(msm_breath_led_exit);

MODULE_DESCRIPTION("MSM CHARGER LEDs driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:breath-leds");


