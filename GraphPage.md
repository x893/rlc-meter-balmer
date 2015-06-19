Один из плюсов данного прибора - возможность измерения на компьютере при подключении к USB. Это позволяет строить графики и смотреть где, что да как.

Во например как выглядит зависимость комплексного сопротивления от частоты у первичной обмотки трансформатора 220 Вольт. Около 5 КГц виден параллельный резонанс, после чего в комплексной части начинает преобладать емкостная составляющая.

![http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/trans_220V.png](http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/trans_220V.png)

А вот так например выглядит комплексное сопротивление у хороших наушников. Небольшая емкостная составляющая на низких частотах - это механическая жесткость мембраны (вероятно).

![http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/phones.png](http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/phones.png)

А вот так выглядит измерение резистора 200 Ом. Ошибка на частотах 100 Гц - 10 КГц порядка 0.1%. На частотах выше 100 КГц начинает сказываться фазовая ошибка, из-за чего ошибка на частоте 175 КГц около 0.5%.

![http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/200Om.png](http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/200Om.png)

А вот так выглядит реальная часть 200 Ом резистора в приближении. На шкале 0.1 - это 200 Ом ровно. 0.2 - это 200.1 Ом.

![http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/200OmRe.png](http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/200OmRe.png)

А вот индуктивность 1 uH. До частоты 5 КГц показания очень шумные. Но вот с частоты 10 КГц, мнимая часть сопротивления становится больше 60 mOm и можно уже верить показаниям с точностью ±5%.

![http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/1uH.png](http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/1uH.png)

Емкость конденстора 1500 pF. В точности показаний до 300 Гц не очень уверен, но вот выше - вполне актуальные данные. На 100 Гц такие конденсаторы уже не получается точно измерять по двум причинам. Сопротивление конденсатора на такой частоте больше 1 МОм. На низких частотах повышается фазовая ошибка. На 100 Гц она порядка 0.2%-0.3%, на частоте 1 КГц она лучше 0.1%. Ступенька на частоте чуть больше 10 КГц связанна с переключением диапазонов измерения.

![http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/1500pF.png](http://wiki.rlc-meter-balmer.googlecode.com/hg/images/graph/1500pF.png)