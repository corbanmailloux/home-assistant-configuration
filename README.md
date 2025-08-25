# Corban Mailloux's Home Assistant Configuration

This is the configuration I use for [Home Assistant](https://www.home-assistant.io/). Currently, I have Home Assistant running on the official [Home Assistant Blue](https://www.home-assistant.io/blue) bundle (basically an [ODROID-N2+](https://www.hardkernel.com/shop/odroid-n2-with-4gbyte-ram-2/) plus eMMC and a pretty case).

## Explanation of Packages/Automations
I use [packages](https://www.home-assistant.io/docs/configuration/packages/) for the vast majority of my YAML configuration to help group projects together. The packages are located [here](https://github.com/corbanmailloux/home-assistant-configuration/tree/master/packages).

With Home Assistant's recent focus on UI configuration over YAML, many of my recent projects are built directly in the UI and not cleanly separated into `packages`. That makes this shared configuration less useful for reference, but it makes quick mobile edits significantly easier. Where appropriate, I use Home Assistant's native category and labeling features to group related automations.
