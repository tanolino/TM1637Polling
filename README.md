# TM1637Polling
I copied code from https://github.com/avishorp/TM1637 and turned it into a polling lib because the bit banging did not work for my project.

## HowTo use
Well this should behave like the original project but flush() is required to write everything (blocking).
If you however want to use the raw bit banging power, check out how flush() calls update() every 100ms until the state is no longer idle().

Note: I read the TM1637 chips can survive 80ms upwards. So the 100ms appears to be an eyeballed value. This information might give you more wiggle room in implementations like my projects, where you can not guarantee exact 100ms ticks.
