breed [ bacteria bacterium ]
breed [ spores spore ]
breed [ botoxes botox ]

spores-own [
  age
]

bacteria-own [
  variant
  energy
  ttl
  feeding-failed
]


patches-own [
  occupant
  glucose-amount
]

globals [
  glucose-per-feed
  acid-amount

  acidity
  temperature

  #-hours

  min-temp-brevis
  max-temp-brevis
  min-temp-plantarum
  max-temp-plantarum
  min-temp-botulinum
  max-temp-botulinum
  min-temp-botox

  min-ph-brevis
  max-ph-brevis
  min-ph-plantarum
  max-ph-plantarum
  min-ph-botulinum
  max-ph-botulinum

  max-ttl
]

;;;;;;;;;; setup procedures ;;;;;;;;;;

to setup
  clear-all

  set glucose-per-feed 0.5

  ;; Static globals obtained from  s c i e n c e

  ;; Temperature ranges, min means minimum for growth, max means maximum before death
  set min-temp-botox 5.0
  set min-temp-brevis 10.0
  set max-temp-brevis 50.0
  set min-temp-plantarum 10.0
  set max-temp-plantarum 45.0
  set min-temp-botulinum 5.0
  set max-temp-botulinum 80.0

  ;; pH ranges, min and max mean for growth, but time spent outside these result
  ;; in death
  set min-ph-brevis 2.5
  set max-ph-brevis 7.0
  set min-ph-plantarum 3.0
  set max-ph-plantarum 8.0
  set min-ph-botulinum 4.6
  set max-ph-botulinum 8.0

  ;; Initially bacteria have maximum "time-to-live". This can be practically any
  ;; large number, it is simply used to differentiate from small numbers
  set max-ttl 100000

  ;; globals that can change during runtime ;;
  set acidity initial-acidity
  set temperature initial-temperature

  ;; Amount of acid when the pH-value is 'initial-acidity'
  set acid-amount calculate-initial-acid-amount

  ;; helper for indicating the passage of time in a human readable format ;;
  set #-hours 0

  set-default-shape spores "dot"
  set-default-shape botoxes "x"
  set-default-shape bacteria "rod"

  setup-patches
  setup-culture

  reset-ticks
end

;; Create bacteria culture
to setup-culture
  ask patches [
    let this-patch self

    ;; Conditionally sprout a bacterium on each patch depending on initial
    ;; chances. Maximum of 1 bacterium per patch
    if random-float 1.0 < initial-brevis-chance [
      sprout-bacteria 1 [
        set variant "L.brevis"
        set color pink
        setup-bacteria-defaults 8.0
      ]
    ]
    if random-float 1.0 < initial-plantarum-chance [
      sprout-bacteria 1 [
        set variant "L.plantarum"
        set color cyan
        setup-bacteria-defaults 8.0
      ]
    ]
    if random-float 1.0 < initial-botulinum-chance [
      sprout-bacteria 1 [
        set variant "C.botulinum"
        set color 13
        setup-bacteria-defaults 8.0
      ]
    ]

    ;; Choose a random bacterium here and purge others
    if any? bacteria-here [
      ask one-of bacteria-here [
        let this-bacterium self
        ask this-patch [ set occupant this-bacterium ]
        ask other bacteria-here [ die ]
      ]
    ]
  ]
end

to setup-patches
  add-glucose
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;; utilities ;;;;;;;;;;;;;;

;; This is quite arbitrarily defined. It was done by visual estimate of what
;; amount of "x"s in the model should correspond to which level of toxicity
to-report calculate-toxicity
   report count botoxes * 2
end

to-report passed-time
  let h word (floor #-hours) "h "
  let m word (remainder ticks 60) "min"
    report word h m
end

to setup-bacteria-defaults [ energy-amount ]
  set feeding-failed false
  set ttl max-ttl
  set energy energy-amount
end

;; Create glucose on patches depending on 'initial-glucose-diffusion'
to add-glucose
  ask patches [
    ifelse random-float 1.0 < initial-glucose-diffusion
      [
        set glucose-amount 10.0
        set pcolor 46
      ]
      [
        if glucose-amount <= 0 [
          set pcolor white
        ]
      ]
  ]
end

;; Curve that roughly correlates to the changing pH value during
;; fermentation
to-report calculate-acidity
  let x acid-amount + 0.33
  report 6.0 - 2 * log x 10
end

;; Solve the above equation for 'acid-amount' when acidity is known
to-report calculate-initial-acid-amount
  report 10 ^ ((acidity - 6.0) / -2) - 0.33
end

;; Produces a negative parabola that goes to zero at a and b. The
;; coefficient is then 'value at x' / 'peak value'
to-report env-coefficient [ a b x ]
  let current -1.0 * (x ^ 2) + (a + b) * x - (a * b)
  let peakX (a + b) / 2
  let peakValue -1.0 * (peakX ^ 2) + (a + b) * peakX - (a * b)
  report current / peakValue
end

to-report get-ph-coefficient
  if variant = "L.brevis" [
    report env-coefficient min-ph-brevis max-ph-brevis acidity
  ]
  if variant = "L.plantarum" [
    report env-coefficient min-ph-plantarum max-ph-plantarum acidity
  ]
  if variant = "C.botulinum" [
    report env-coefficient min-ph-botulinum max-ph-botulinum acidity
  ]
end

to-report get-temp-coefficient
  if variant = "L.brevis" [
    report env-coefficient min-temp-brevis max-temp-brevis temperature
  ]
  if variant = "L.plantarum" [
    report env-coefficient min-temp-plantarum max-temp-plantarum temperature
  ]
  if variant = "C.botulinum" [
    report env-coefficient min-temp-botulinum max-temp-botulinum temperature
  ]
end

to-report get-max-temp
  if variant = "L.brevis" [
    report max-temp-brevis
  ]
  if variant = "L.plantarum" [
    report max-temp-plantarum
  ]
  if variant = "C.botulinum" [
    report max-temp-botulinum
  ]
end

;; Check if a bacterium is going to start dying. This means the bacterium will
;; live for another 'ttl-value' ticks
to-report calculate-possible-ttl
  let ph-coeff get-ph-coefficient
  let max-temp get-max-temp


  let ttl-value ttl

  if ttl-value != max-ttl [
    set ttl-value ttl-value - 1
  ]
  if ttl >= 80 and (ph-coeff <= 0 or temperature - max-temp >= 0) [
    set ttl-value 50 + random 30
  ]
  if ttl >= 30 and (ph-coeff <= -0.5 or temperature - max-temp >= 10) [
    set ttl-value 10 + random 20
  ]
  if ph-coeff <= -1 or temperature - max-temp >= 20 [
    set ttl-value random 5
  ]

  report ttl-value
end

;; Remove glucose from patch and report the amount to be fed to a bacterium
to-report yield-glucose
  if glucose-amount = 0 [ report 0 ]

  set glucose-amount glucose-amount - glucose-per-feed
  map-glucose-to-color

  ifelse glucose-amount <= 0
    [ report glucose-per-feed + glucose-amount ]
    [ report glucose-per-feed ]

end

to map-glucose-to-color
  if glucose-amount <= 0 [ set pcolor white ]
  if glucose-amount > 0.0 [ set pcolor 49 ]
  if glucose-amount > 2.5 [ set pcolor 48 ]
  if glucose-amount > 5.0 [ set pcolor 47 ]
  if glucose-amount > 7.5 [ set pcolor 46 ]
end

;; Metabolize 'fed-amount' of glucose
to metabolize [ fed-amount ]
  set energy energy - 0.2
  let ph-coeff get-ph-coefficient
  let temp-coeff get-temp-coefficient

  ;; C.botulinum has a chance to produce botulinum toxin (botox) during
  ;; metabolism while the other bacteria produce lactic acid
  ifelse variant = "C.botulinum"
    [
      if random-float 1.0 < ph-coeff * temp-coeff [
        ask patch-here [
          sprout-botoxes 1 [
            set color 12
            set size 0.5
          ]
        ]
      ]
    ]
    [ set acid-amount acid-amount + fed-amount / 250 ]
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;; runtime procedures ;;;;;;;;;

to go
  tick
  update-time
  ask bacteria [ perform-activity ]
  ask spores [ grow-spore ]
  set acidity calculate-acidity
end

to update-time
  ;; One tick = 1 minute in simulated time
  set #-hours ticks / 60
end

;; Bacteria do things depending on their current energy level. 
;; Normally, they move around. With low energy they eat and with high energy
;; they reproduce
to perform-activity
  if ttl = 0 [ die ]

  set ttl calculate-possible-ttl

  let ph-coeff get-ph-coefficient
  let temp-coeff get-temp-coefficient

  if energy <= 0 or random-float 1.0 <= 1 - ph-coeff or random-float 1.0 <= 1 - temp-coeff [
    stop
  ]

  if feeding-failed [
    move-bacterium
    set feeding-failed false
    stop
  ]
  if energy <= 2.0 [
    try-eat-glucose
    stop
  ]
  ;; Decrease requirements for reproduction as a function of the suitability
  ;; of the environment
  if energy > 10 - 5 * ph-coeff and energy > 10 - 5 * temp-coeff [
    reproduce
    stop
  ]

  ;; "Default" to moving around
  ifelse random-float 1.0 <= 0.9
    [ move-bacterium ]
    [ try-eat-glucose ]


end

to move-bacterium

  ;; Check for nearby bacterium
  let nearby-bacteria other bacteria in-radius 1

  ;; Turn (at least) orthogonally wrt. the closest of the nearby bacteria
  ifelse any? nearby-bacteria
  [
    face min-one-of nearby-bacteria [ distance myself ]
    let deg random 90 + 90
    ifelse random 2 = 0
      [right deg]
      [left deg]
  ]
  ;; Otherwise wiggle between -5 and 5 deg
  [
    ifelse random 2 = 0
      [right random 5]
      [left random 5]
  ]

  set energy energy - 0.05

  forward 0.2
end

;; A spore turns into a C.botulinum bacterium after 40 ticks
to grow-spore
  if age >= 40 [
    hatch-bacteria 1 [
      set color 13
      set variant "C.botulinum"
      setup-bacteria-defaults 4.0
    ]
    die
  ]
  set age age + 1
end

to reproduce
  let parent-variant variant
  let shared-energy energy / 2
  set energy shared-energy

  if variant = "C.botulinum" [
    hatch-spores 1 [
      set color 13
      set age 0
    ]
    stop
  ]

  hatch-bacteria 1 [
    set variant parent-variant
    setup-bacteria-defaults shared-energy
    if variant = "L.brevis" [
      set color pink
    ]
    if variant = "L.plantarum" [
      set color cyan
    ]
  ]
end

;; Try to eat glucose in the current patch
to try-eat-glucose
  let fed-amount 0
  ask patch-here [ set fed-amount yield-glucose ]
  ifelse fed-amount > 0
    [
      set energy energy + 2 * fed-amount
      metabolize fed-amount
      set feeding-failed false
    ]
    [ set feeding-failed true ]
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
@#$#@#$#@
GRAPHICS-WINDOW
293
93
730
531
-1
-1
13.0
1
10
1
1
1
0
1
1
1
-16
16
-16
16
1
1
0
ticks
30.0

BUTTON
124
93
224
126
Run model
go
T
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
32
93
101
126
Setup
setup
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

TEXTBOX
31
190
244
228
Chances for spawning either glucose or a type of bacteria
14
0.0
1

SLIDER
31
238
244
271
initial-glucose-diffusion
initial-glucose-diffusion
0.0
1.0
0.6
0.1
1
NIL
HORIZONTAL

SLIDER
31
286
244
319
initial-brevis-chance
initial-brevis-chance
0.0
0.1
0.04
0.01
1
NIL
HORIZONTAL

SLIDER
31
334
244
367
initial-plantarum-chance
initial-plantarum-chance
0.0
0.1
0.04
0.01
1
NIL
HORIZONTAL

SLIDER
33
382
244
415
initial-botulinum-chance
initial-botulinum-chance
0.0
0.1
0.02
0.01
1
NIL
HORIZONTAL

TEXTBOX
31
439
241
462
Starting point for environment
14
0.0
1

SLIDER
31
469
208
502
initial-acidity
initial-acidity
2.0
7.0
6.0
0.1
1
NIL
HORIZONTAL

SLIDER
31
517
208
550
initial-temperature
initial-temperature
5.0
60.0
25.0
1
1
NIL
HORIZONTAL

PLOT
771
92
1017
285
pH-value
Time (h)
pH
0.0
24.0
0.0
8.0
true
false
"" ""
PENS
"default" 1.0 0 -6565750 true "" "plotxy #-hours acidity"

BUTTON
31
143
141
176
Add glucose
add-glucose
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

PLOT
771
308
1285
527
Bacteria populations
Time (h)
Count
0.0
24.0
0.0
300.0
true
true
"" ""
PENS
"L.plantarum" 1.0 0 -11221820 true "" "plotxy #-hours count bacteria with [ variant = \"L.plantarum\" ]"
"L.brevis" 1.0 0 -2064490 true "" "plotxy #-hours count bacteria with [ variant = \"L.brevis\" ]"
"C.botulinum" 1.0 0 -8053223 true "" "plotxy #-hours count bacteria with [ variant = \"C.botulinum\" ]"

MONITOR
293
15
440
80
Time
passed-time
17
1
16

PLOT
1038
93
1285
284
Toxicity
Time (h)
Toxin ng/kg
0.0
24.0
0.0
1500.0
true
false
"" ""
PENS
"toxicity" 1.0 0 -10873583 true "" "plotxy #-hours calculate-toxicity"
"lethal" 1.0 0 -1604481 true "" "plotxy 0 800\nplotxy 24 800\nplotxy #-hours 800"

@#$#@#$#@
## WHAT IS IT?

This model studies the growth of certain bacteria in the Lactobacillus family
during lactic acid fermentation. Particularly, the Levilactobacillus brevis (L.
brevis) and Lactiplantibacillus plantarum (L. plantarum) strains which are
highly prevalent when fermenting, for example, sauerkraut, kimchi or pickles. In
addition, another bacterium, Clostridium botulinum (C. botulinum), is modeled,
as it is present in many raw vegetables. C. botulinum can produce botulinum
toxin (botox) which causes a severe disease called botulism. Lactic acid
fermentation is an effective way to prevent the growth of C. botulinum, which is
one of the reasons for the ubiquity of fermentation as a method of food
preservation throughout history.

The growth of the Lactobacillus bacteria depend on several factors in their
environment. The most important of these are the presence of a source of carbon,
generally glucose, the temperature of the environment, and the acidity of the
environment. The growth of C.botulinum, on the other hand, is mainly affected by
the acidity of the environment. It is also affected by the temperature, but is
highly resistant to changes in it.

## HOW IT WORKS

L.brevis (pink), L.plantarum (cyan) and C.botulinum (dark red) bacteria appear
on a substrate which contains glucose. During a simulation round, the bacteria
either move around, eat (and metabolize) glucose or reproduce. Their capability
for doing these activities is modeled by them having an "energy" which
increases when eating and decreases with movement and reproduction. L.brevis and
L.plantarum reproduce simply by cloning themselves and giving half their energy
to the new bacterium. C.botulinum, however, is a spore-producing species of
bacteria, meaning that initially inactive spores are produced. Over some time
(40 minutes in this model), the spores grow to new bacteria.

When either L.brevis or L.plantarum metabolize, they produce lactic acid,
increasing the acidity of the environment. This then affects the behavior of all
the bacteria as each species has their own optimal operating environment. In
practice, each species has a minimum and maximum pH value and a minimum and
maximum temperature value. A coefficient is calculated from these for every
timestep that affects many of the behaviors that they articulate.

When C.botulinum metabolizes, it has a chance to produce botox depending on the
current temperature and acidity. Once produced, the botox remains in the model
forever. This follows the real-life lifespan of botox as it can only be removed
with boiling water.

Both the acidity and toxicity of the environment are plotted as a function of
the time passed in hours where one tick is one minute. The sizes of the bacteria
populations are similarly plotted.

## HOW TO USE IT

The simplest way to use the model is to first press "Setup" and then "Run
model". The model will then start simulating the behavior of the bacteria. The
activity in the model will continue until roughly 11 hours of simulated time.

Taking it further, one can tweak the starting configuration of the model in a
few different ways. First, changing the value of the "initial-glucose-diffusion" 
slider, the amount of glucose in the model changes when pressing "Setup". The
amount of glucose can also be changed during simulation by pressing the "Add
glucose" button. This further populates the world with glucose, also depending on the
"initial-glucose-diffusion" value.

The next three sliders tweak the initial chances for spawning a certain species
of bacteria. In practice, during setup, a dice is rolled for each patch to see
if any bacteria will appear there. The "initial-<bacteria-species>-chance" value
specifies the odds at which a member of that species will appear on the patch.

Lastly, the "initial-acidity" and "initial-temperature" sliders are fairly
self-explanatory. They set up the pH-value and the degrees of temperature (ºC)
in the environment. Note, though, that a higher value on the "initial-acidity"
slider corresponds to a higher pH, and a lower level of acidity. Yes, it's
somewhat counterintuitive. 

These value, however, are only the initial values. They cannot be changed when
running the model.

## THINGS TO NOTICE

The initial configuration describes a scenario typical for a fairly early phase
of fermentation. Glucose is plentiful, acidity is relatively low and bacteria
populations are small. At first, due to the low acidity, C.botulinum has a
chance to produce botox and sporulate, leading to more bacteria present. This
chance is quickly removed as the L.brevis and L.plantarum produce lactic acid
and drive the acidity down below 4.6 pH (a critical point for C.botulinum). The
C.botulinum population crashes and the increase of toxicity stops. 

Here, L.plantarum thrives, as the environment is close to its optimum. The
bacteria count increases and more and more bacteria exist to create more lactic
acid. This, however, makes the environment less hospitable for L.plantarum and
more for L.brevis, which prefers lower pH levels. The growth of L.brevis
accelerates and the population passes that of L.plantarum. The growth of both
peter out after 8 or so hours when the glucose has mostly been eaten. After
this, their populations remain stable, but inactive due to the bacteria not
having enough energy for mobility.

The end product is a ferment with fairly high acidity, slightly above 3 pH. This
is not quite realistic as real fermentation would generally reach levels between
3.5 pH and 4.5 pH. Some toxicity is present in the environment, but it is well
below dangerous levels in terms of oral consumption (as indicated by the red
line in the "Toxicity" plot). This indicates that the ferment would be safe to
eat.

## THINGS TO TRY

Especially changing the "initial-acidity" and "initial-temperature" values cause
some interesting effects. A slightly lower initial acidity will lead to the
L.plantarum population eventually crashing and L.brevis taking the throne as
L.brevis is slightly more tolerant to high acidity. A higher temperature,
however, quickly leads to a more toxic environment, as C.botulinum has better
chances for growth and botox production. 

Tweaking the chances for specific bacteria spawning change the assumption of the
starting point. For example, higher chances for spawning L.brevis or L.plantarum
would indicate that the ferment is further along its course. A higher chance of
C.botulinum spawning would describe a situation where the fermented substance
had been stored poorly (in high temperatures) before fermentation.

## EXTENDING THE MODEL

The model contains quite a bit of "magic numbers", meaning constant values that
were introduced fairly arbitrarily simply in attempt to have the model
correspond more to reality. These are, for example, how much glucose is removed
from the environment when feeding and how much energy is gained in response,
how much energy is required for movement, how long bacteria survive outside
their minimum and maximum environmental conditions, et cetera. There will surely
be values for these that move the model closer towards reality, but also many
values that simply create interesting effects. It is worth it to try and change
those!

This model was also built with a very basic understanding of the behavior of
bacteria. A more academic approach would surely result in a more accurate model.
However, it has been fun to build this model and already at this level it is
showing interesting emergent phenomena!

## RELATED MODELS

See e.g. the "Bacterial Infection" model in NetLogo.

## CREDITS AND REFERENCES

Giraud, E., Lelong, B. & Raimbault, M. (1991), ‘Influence of ph and initial lactate concen-
tration on the growth of lactobacillus plantarum’, Applied microbiology and biotech-
nology 36, 96–99.

Matejčeková, Z., Liptáková, D., Spodniaková, S. & Valík, L. (2016), ‘Characterization of the
growth of in milk in dependence on temperature’, Acta Chimica Slovaca 9(2), 104–108.

Odlaug, T. E. & Pflug, I. J. (1978), ‘Clostridium botulinum and acid foods’, Journal of Food
Protection 41(7), 566–573.

Saarela, M., Rantala, M., Hallamaa, K., Nohynek, L., Virkajärvi, I. & Mättö, J. (2004),
‘Stationary-phase acid and heat treatments for improvement of the viability of pro-
biotic lactobacilli and bifidobacteria’, Journal of Applied Microbiology 96(6), 1205–1214.

@#$#@#$#@
default
true
0
Polygon -7500403 true true 150 5 40 250 150 205 260 250

airplane
true
0
Polygon -7500403 true true 150 0 135 15 120 60 120 105 15 165 15 195 120 180 135 240 105 270 120 285 150 270 180 285 210 270 165 240 180 180 285 195 285 165 180 105 180 60 165 15

arrow
true
0
Polygon -7500403 true true 150 0 0 150 105 150 105 293 195 293 195 150 300 150

box
false
0
Polygon -7500403 true true 150 285 285 225 285 75 150 135
Polygon -7500403 true true 150 135 15 75 150 15 285 75
Polygon -7500403 true true 15 75 15 225 150 285 150 135
Line -16777216 false 150 285 150 135
Line -16777216 false 150 135 15 75
Line -16777216 false 150 135 285 75

bug
true
0
Circle -7500403 true true 96 182 108
Circle -7500403 true true 110 127 80
Circle -7500403 true true 110 75 80
Line -7500403 true 150 100 80 30
Line -7500403 true 150 100 220 30

butterfly
true
0
Polygon -7500403 true true 150 165 209 199 225 225 225 255 195 270 165 255 150 240
Polygon -7500403 true true 150 165 89 198 75 225 75 255 105 270 135 255 150 240
Polygon -7500403 true true 139 148 100 105 55 90 25 90 10 105 10 135 25 180 40 195 85 194 139 163
Polygon -7500403 true true 162 150 200 105 245 90 275 90 290 105 290 135 275 180 260 195 215 195 162 165
Polygon -16777216 true false 150 255 135 225 120 150 135 120 150 105 165 120 180 150 165 225
Circle -16777216 true false 135 90 30
Line -16777216 false 150 105 195 60
Line -16777216 false 150 105 105 60

car
false
0
Polygon -7500403 true true 300 180 279 164 261 144 240 135 226 132 213 106 203 84 185 63 159 50 135 50 75 60 0 150 0 165 0 225 300 225 300 180
Circle -16777216 true false 180 180 90
Circle -16777216 true false 30 180 90
Polygon -16777216 true false 162 80 132 78 134 135 209 135 194 105 189 96 180 89
Circle -7500403 true true 47 195 58
Circle -7500403 true true 195 195 58

circle
false
0
Circle -7500403 true true 0 0 300

circle 2
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240

cow
false
0
Polygon -7500403 true true 200 193 197 249 179 249 177 196 166 187 140 189 93 191 78 179 72 211 49 209 48 181 37 149 25 120 25 89 45 72 103 84 179 75 198 76 252 64 272 81 293 103 285 121 255 121 242 118 224 167
Polygon -7500403 true true 73 210 86 251 62 249 48 208
Polygon -7500403 true true 25 114 16 195 9 204 23 213 25 200 39 123

cylinder
false
0
Circle -7500403 true true 0 0 300

dot
false
0
Circle -7500403 true true 90 90 120

face happy
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 255 90 239 62 213 47 191 67 179 90 203 109 218 150 225 192 218 210 203 227 181 251 194 236 217 212 240

face neutral
false
0
Circle -7500403 true true 8 7 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Rectangle -16777216 true false 60 195 240 225

face sad
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 168 90 184 62 210 47 232 67 244 90 220 109 205 150 198 192 205 210 220 227 242 251 229 236 206 212 183

fish
false
0
Polygon -1 true false 44 131 21 87 15 86 0 120 15 150 0 180 13 214 20 212 45 166
Polygon -1 true false 135 195 119 235 95 218 76 210 46 204 60 165
Polygon -1 true false 75 45 83 77 71 103 86 114 166 78 135 60
Polygon -7500403 true true 30 136 151 77 226 81 280 119 292 146 292 160 287 170 270 195 195 210 151 212 30 166
Circle -16777216 true false 215 106 30

flag
false
0
Rectangle -7500403 true true 60 15 75 300
Polygon -7500403 true true 90 150 270 90 90 30
Line -7500403 true 75 135 90 135
Line -7500403 true 75 45 90 45

flower
false
0
Polygon -10899396 true false 135 120 165 165 180 210 180 240 150 300 165 300 195 240 195 195 165 135
Circle -7500403 true true 85 132 38
Circle -7500403 true true 130 147 38
Circle -7500403 true true 192 85 38
Circle -7500403 true true 85 40 38
Circle -7500403 true true 177 40 38
Circle -7500403 true true 177 132 38
Circle -7500403 true true 70 85 38
Circle -7500403 true true 130 25 38
Circle -7500403 true true 96 51 108
Circle -16777216 true false 113 68 74
Polygon -10899396 true false 189 233 219 188 249 173 279 188 234 218
Polygon -10899396 true false 180 255 150 210 105 210 75 240 135 240

glucose
false
0
Rectangle -7500403 true true 0 0 300 300

house
false
0
Rectangle -7500403 true true 45 120 255 285
Rectangle -16777216 true false 120 210 180 285
Polygon -7500403 true true 15 120 150 15 285 120
Line -16777216 false 30 120 270 120

leaf
false
0
Polygon -7500403 true true 150 210 135 195 120 210 60 210 30 195 60 180 60 165 15 135 30 120 15 105 40 104 45 90 60 90 90 105 105 120 120 120 105 60 120 60 135 30 150 15 165 30 180 60 195 60 180 120 195 120 210 105 240 90 255 90 263 104 285 105 270 120 285 135 240 165 240 180 270 195 240 210 180 210 165 195
Polygon -7500403 true true 135 195 135 240 120 255 105 255 105 285 135 285 165 240 165 195

line half
true
0
Line -7500403 true 150 0 150 150

pentagon
false
0
Polygon -7500403 true true 150 15 15 120 60 285 240 285 285 120

person
false
0
Circle -7500403 true true 110 5 80
Polygon -7500403 true true 105 90 120 195 90 285 105 300 135 300 150 225 165 300 195 300 210 285 180 195 195 90
Rectangle -7500403 true true 127 79 172 94
Polygon -7500403 true true 195 90 240 150 225 180 165 105
Polygon -7500403 true true 105 90 60 150 75 180 135 105

plant
false
0
Rectangle -7500403 true true 135 90 165 300
Polygon -7500403 true true 135 255 90 210 45 195 75 255 135 285
Polygon -7500403 true true 165 255 210 210 255 195 225 255 165 285
Polygon -7500403 true true 135 180 90 135 45 120 75 180 135 210
Polygon -7500403 true true 165 180 165 210 225 180 255 120 210 135
Polygon -7500403 true true 135 105 90 60 45 45 75 105 135 135
Polygon -7500403 true true 165 105 165 135 225 105 255 45 210 60
Polygon -7500403 true true 135 90 120 45 150 15 180 45 165 90

rod
true
0
Polygon -7500403 true true 135 45
Polygon -7500403 true true 135 15 120 30 120 270 135 285 165 285 180 270 180 30 165 15

sheep
false
15
Circle -1 true true 203 65 88
Circle -1 true true 70 65 162
Circle -1 true true 150 105 120
Polygon -7500403 true false 218 120 240 165 255 165 278 120
Circle -7500403 true false 214 72 67
Rectangle -1 true true 164 223 179 298
Polygon -1 true true 45 285 30 285 30 240 15 195 45 210
Circle -1 true true 3 83 150
Rectangle -1 true true 65 221 80 296
Polygon -1 true true 195 285 210 285 210 240 240 210 195 210
Polygon -7500403 true false 276 85 285 105 302 99 294 83
Polygon -7500403 true false 219 85 210 105 193 99 201 83

square 2
false
0
Rectangle -7500403 true true 30 30 270 270
Rectangle -16777216 true false 60 60 240 240

star
false
0
Polygon -7500403 true true 151 1 185 108 298 108 207 175 242 282 151 216 59 282 94 175 3 108 116 108

target
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240
Circle -7500403 true true 60 60 180
Circle -16777216 true false 90 90 120
Circle -7500403 true true 120 120 60

tree
false
0
Circle -7500403 true true 118 3 94
Rectangle -6459832 true false 120 195 180 300
Circle -7500403 true true 65 21 108
Circle -7500403 true true 116 41 127
Circle -7500403 true true 45 90 120
Circle -7500403 true true 104 74 152

triangle
false
0
Polygon -7500403 true true 150 30 15 255 285 255

triangle 2
false
0
Polygon -7500403 true true 150 30 15 255 285 255
Polygon -16777216 true false 151 99 225 223 75 224

truck
false
0
Rectangle -7500403 true true 4 45 195 187
Polygon -7500403 true true 296 193 296 150 259 134 244 104 208 104 207 194
Rectangle -1 true false 195 60 195 105
Polygon -16777216 true false 238 112 252 141 219 141 218 112
Circle -16777216 true false 234 174 42
Rectangle -7500403 true true 181 185 214 194
Circle -16777216 true false 144 174 42
Circle -16777216 true false 24 174 42
Circle -7500403 false true 24 174 42
Circle -7500403 false true 144 174 42
Circle -7500403 false true 234 174 42

turtle
true
0
Polygon -10899396 true false 215 204 240 233 246 254 228 266 215 252 193 210
Polygon -10899396 true false 195 90 225 75 245 75 260 89 269 108 261 124 240 105 225 105 210 105
Polygon -10899396 true false 105 90 75 75 55 75 40 89 31 108 39 124 60 105 75 105 90 105
Polygon -10899396 true false 132 85 134 64 107 51 108 17 150 2 192 18 192 52 169 65 172 87
Polygon -10899396 true false 85 204 60 233 54 254 72 266 85 252 107 210
Polygon -7500403 true true 119 75 179 75 209 101 224 135 220 225 175 261 128 261 81 224 74 135 88 99

wheel
false
0
Circle -7500403 true true 3 3 294
Circle -16777216 true false 30 30 240
Line -7500403 true 150 285 150 15
Line -7500403 true 15 150 285 150
Circle -7500403 true true 120 120 60
Line -7500403 true 216 40 79 269
Line -7500403 true 40 84 269 221
Line -7500403 true 40 216 269 79
Line -7500403 true 84 40 221 269

wolf
false
0
Polygon -16777216 true false 253 133 245 131 245 133
Polygon -7500403 true true 2 194 13 197 30 191 38 193 38 205 20 226 20 257 27 265 38 266 40 260 31 253 31 230 60 206 68 198 75 209 66 228 65 243 82 261 84 268 100 267 103 261 77 239 79 231 100 207 98 196 119 201 143 202 160 195 166 210 172 213 173 238 167 251 160 248 154 265 169 264 178 247 186 240 198 260 200 271 217 271 219 262 207 258 195 230 192 198 210 184 227 164 242 144 259 145 284 151 277 141 293 140 299 134 297 127 273 119 270 105
Polygon -7500403 true true -1 195 14 180 36 166 40 153 53 140 82 131 134 133 159 126 188 115 227 108 236 102 238 98 268 86 269 92 281 87 269 103 269 113

x
false
0
Polygon -7500403 true true 270 75 225 30 30 225 75 270
Polygon -7500403 true true 30 75 75 30 270 225 225 270
@#$#@#$#@
NetLogo 6.4.0
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
default
0.0
-0.2 0 0.0 1.0
0.0 1 1.0 0.0
0.2 0 0.0 1.0
link direction
true
0
Line -7500403 true 150 150 90 180
Line -7500403 true 150 150 210 180
@#$#@#$#@
0
@#$#@#$#@
