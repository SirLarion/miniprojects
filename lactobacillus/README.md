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
