# LMeter
Simple Arduino nano based inductor tester


![Prototype](20210511_145031.jpg "Prototype")
![Prototype_1](20210511_144249.jpg "Prototype Rear")

![Schematic](LMeter.pdf?raw=true "Schematic")

After measuring inductors with a function generator, an oscilloscope and a calculator, I decided that I needed a quicker way to measure inductor values, from ~1uH to ~100uH (or from 1uH to as high as I can manage).

I stumbled upon this project (https://skootsone.yolasite.com/l-meter.php), which inspired me to actually cobble something quick and dirty and 'good enough' together. I don't have any mechanical meter movements around, and realized that buying one plus a cheap solar light to scavange from would cost more than using a ![$1 arduino clone board](https://github.com/dbuezas/lgt8fx "LGT8F328p") and character LCD. And, I already have those in my parts bin.

The concept is simple enough - apply a square wave (I settled on 200kHz) to an inductor, and build a crude boost converter. Then measure the voltage drop produced across a load. The rest is all details, which I crudely banged together mostly by guess-and-test, with the skootsone design values as my starting point. I picked the blue LED/1k resistor as a load after playing around to find something that produces a voltage response that most closely fits within my ADC's range. I powered the boost converter from a 1.2V source, to keep the maximum voltage produced down, and to ensure that I have a fairly stable reference voltage for testing. I settled on a 33uH inductor, which also limits the maximum voltage output (though, a 22 or 27uH might be a better fit). I cobbled things up on a solderless breadboard, and fiddled around with component values until I produced something which worked okay. I may make some further changes to component values in order to get a response curve that provides the best resolution over the range of inductors that I plan to test. I may also implement some DC signal conditioning with an opamp to make bestuse of my ADC's range (and not waste bits measuring values I'm not interested in)

Using a microcontroller (as opposed to a mechanical movement) does give me a bit of additional flexability to potentially extend functionality down the road. For example, I've seen some methods for measuring saturation current, which I might eventually implement here.

The end result of all of this is a circuit which produces an output of roughly 0V (infine inductance / open circuit) to 3.6V (zero inductance / short circuit), which is sampled by the ADC. Right now, I'm just spitting that raw value out to the LCD. Next, I need to measure out a bunch of different inductor values, and plot out a response curve. Hopefully, I can approximate a function that describes the curve - then I'll have the uC do some math, and spit out a result in uH. Otherwise, I'll build a mapping table to use.

I scanned about a hundred labelled +/-20% inductors from my kit, averaged out the values received for each increment, and charted them out:
|Labelled Inductor Value|Average ADC reading|
|:-:|:-:|
|0|3060|
|.27|3049|
|.56|3038|
|1|3033|
|2.2|2990|
|2.7|2996|
|4.7|2931|
|5.6|2919|
|6.8|2888|
|10|2811|
|22|2537|
|33|2408|
|47|2192|
|68|1992|
|100|1722|
|150|1541|
|220|1238|
|330|1008|
|470|770|
|560|697|
|1000|514|
|2200|572|
|10000|554|

I found a curve fitting program online, and it approximated a function of: "y = 21337840*e^(-1*(x+12785.47)^2/(2*2927.016^2))" for values between 10uH and 470uH. Coding that into the uC, I now get an approximated reading, in uH, for inductors from 10uH to about 500uH.

I can adjust the square wave frequency programatically, so, I might be able to fine tune things for inductors <10uH and >500uH by adjusting the frequency when the ADC reading is below or above a certain threshold. Future tweaks....
