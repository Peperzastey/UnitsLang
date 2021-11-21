# tkom-SIunits

Celem projektu jest implementacja interpretera języka programowania.

Przedstawiony język programowania ma umożliwiać wykonywanie operacji arytmetycznych na wartościach liczbowych z jednostkami z układu SI,
wraz ze sprawdzaniem poprawności operacji na poziomie zgodności jednostek operandów.
Umożliwia definiowanie własnych funkcji celem reużycia fragmentów kodu, obsługuje rekurencję i zmienne lokalne funkcji.

Typowanie w przedstawionym języku jest statyczne i silne.

## Przykładowy kod źródłowy
```
length = 2cm
width = 500 mm

area = 14m2 + (length * width) - 10N / 6Pa
print( "Powierzchnia wynosi {area}, czyli {area in cm2}" )
// stdout: Powierzchnia wynosi 15.677m2, czyli 156 766.667cm2

// wynik domyślnie jest wyświelany z przedrostkiem wielokrotności
// obecnym w układzie SI dla danej jednostki - czyli poza kg wszystkie
// jednostki są domyślnie bez przedrostków
print( "Escapowanie: \{ 1Pa = 1N/1m2 \}" )

// istnieje możliwość podania liczby w grupach 3cyfrowych oddzielonych spacją
mass = 23 001 250.5mg
// \ na końcu linii umożliwia kontynuację instrukcji w następnej linii
calculated_work = (mass * 2 * area * 2.3km) \
    / (length * 5min * 2 000us)
print( "To wyliczenie pewnie nie ma sensu, ale energia wyniosła" \
    "{calculated_work}" )

area = 500cm2
acc = 2 m/s2
dist = acc * 0.5h * 2s
speed = dist * 15Hz

pressure1 = 5kg/(m*s2)
pressure2 = 14 432 kg/m/s2

length = 2.5cm
width = 2cm
area = length * width
while area < 2m2 {
    width = width * 1.5
    area = length * width
}

if area > 3m2 {
    print( "Area is way too big" )
} else if area > 2.5m2 {
    print( "Area is slightly too big" )
} else {
    print( "Area is acceptable" )
}

// parametry funkcji przekazywane są przez wartość
func fibonacci_meters (steps [1]) -> m {
    // indentacja bloku nie jest wymagana
    if steps == 0 {
        return 0m
    } else if steps == 1 {
        return 1m
    } else {
        // rekursja
        return fibonacci_meters(steps - 1) + fibonacci_meters(steps - 2)
    }
}

func fibonacci_speed_iterative (steps [1]) -> m/s {
    // zmienne lokalne
    elem0 = 0m/s
    elem1 = 1m/s
    while steps > 0 {
        temp = elem1
        elem1 = elem0 + elem1
        elem0 = elem1
        steps--
        // ++ i -- możliwe tylko dla wartości bez jednostek ([1])
    }
    return elem1
}

meters_after_16_steps = fibonacci_meters(16)
// możliwość nadania z góry typu dla zmiennej, w tym wypadku jeśli
// wartość przypisywanego wyrażenia nie ma zgodnego typu - rzucony
// będzie błąd
distance[m] = fibonacci_speed_iterative(20) * 16s
// elem0, elem1 i temp z ciała funkcji fibonacci_speed_iterative nie są tu widoczne
```

Przykłady nieprawidłowych operacji (niezgodność jednostek):
```
15m + 3s
3 000s2 - 15s * 2ms / 0.5s
```

## Obsługiwane jednostki
Lista obsługiwanych jednostek wraz z zależnościami pomiędzy nimi:
* `s` (sekunda)
* `min = 60s` (minuta)
* `h = 60min` (godzina)
* `Hz = 1 / s` (herc)
* `g` (gram)
* `m` (metr)
* `N = kg * m / s2` (newton)
* `Pa = kg / (m * s2)` (pascal)
* `J = kg * m2 / s2` (dżul)

## Obsługiwane przedrostki jednostek
* `T  = 1 000 000 000 000x` (tera)
* `G  = 1 000 000 000x` (giga)
* `M  = 1 000 000x` (mega)
* `k  = 1 000x` (kilo)
* `h  = 100x` (hekto)
* `da = 10x` (deka)
* \-
* `d = 0.1x` (decy)
* `c = 0.01x` (centy)
* `m = 0.001x` (mili)
* `u = 0.000 001x` (mikro)
* `n = 0.000 000 001x` (nano)
* `p = 0.000 000 000 001x` (piko)

## Opis gramatyki

Symbole rozpoznawane przez lekser:
```
id                                      = ( litera | '_' ), { litera | cyfra | '_' } ;

koniec_linii                            = '\n' | '\r\n' ;

koniec_instrukcji                       = ? koniec_linii niepoprzedzony sekwencją ( '\', { whitespace } ) ? ;
                                        (* \ na końcu linii umożliwia kontynuację instrukcji w następnej linii *)

cyfra                                   = ? regex [0-9] ? ;

cyfra_niezerowa                         = ? regex [1-9] ? ;

litera                                  = ? regex [a-zA-Z] ? ;

separator_grupy_cyfr                    = ' ' ;
                                        (* spacja *)

liczba                                  = cyfra, [ '.', cyfra, { cyfra } ]
                                        | cyfra_niezerowa, { cyfra }, [ '.', cyfra, { cyfra } ]
                                        | cyfra_niezerowa, 2 * [ cyfra ], { separator_grupy_cyfr, 3 * cyfra }, [ '.', cyfra, { cyfra } ] ;

jednostka_podstawowa                    = 's' | 'min' | 'h' | 'Hz' | 'g' | 'm' | 'N' | 'Pa' | 'J' ;

prefix                                  = 'T' | 'G' | 'M' | 'k' | 'h' | 'da' | 'd' | 'c' | 'm' | 'u' | 'n' | 'p' ;

add_op                                  = '+' | '-' ;

mult_op                                 = '*' | '/' ;

rel_op                                  = '==' | '!=' | '<' | '>' | '<=' | '>=' ;

sufiks_op                               = '++' | '--' ;

znak                                    = ? dowolny znak ASCII - '\n' ? ;

escapowany_cudzysłów                    = '\"' ;

wartość_logiczna                        = 'true' | 'false' ;

skalar                                  = '1' ;

komentarz                               = ? linia zaczynająca się sekwencją '//' ? ;
```

Symbole rozpoznawane przez parser:
```
Symbol startowy:
kod                                     = {
                                          instrukcja
                                        | komentarz 
                                        } ;

tekst                                   = { znak } ;

instrukcja                              = [
                                          definicja_zmiennej
                                        | przypisanie
                                        | definicja_funkcji
                                        | wywołanie_funkcji
                                        | instrukcja_return
                                        | instrukcja_sufiksowa
                                        | pętla_while
                                        | instrukcja_if
                                        | 'break'
                                        | 'continue'
                                        ], koniec_instrukcji ;

definicja_zmiennej                      = id, [ oznaczenie_typu ], '=', wyrażenie ;

oznaczenie_typu                         = '[', typ, ']' ;

typ                                     = jednostka | skalar | 'bool' ;

wyrażenie                               = wyrażenie_addytywne
                                        | wyrażenie_logiczne ;

wyrażenie_addytywne                     = wyrażenie_multiplikatywne, { add_op, wyrażenie_multiplikatywne } ;

wyrażenie_multiplikatywne               = składnik, { mult_op, składnik } ;

składnik                                = id
                                        | wartość
                                        | wywołanie_funkcji
                                        | '(' wyrażenie_addytywne ')' ;

wartość                                 = liczba, [ jednostka ] ;
                                        (* brak jednostki oznacza skalara *)

wyrażenie_logiczne                      = wyrażenie, rel_op, wyrażenie
                                        | wartość_logiczna ;

jednostka                               = jednostka_prosta
                                        | jednostka_złożona ;

jednostka_prosta                        = [ prefix ], ( jednostka_podstawowa | potega_jednostki ) ;

potega_jednostki                        = jednostka_podstawowa, ( '2' | '3' ) ;

jednostka_złożona                       = wyrażenie_multiplikatywne_jednostkowe ;

wyrażenie_multiplikatywne_jednostkowe   = składnik_jednostkowy, { mult_op, składnik_jednostkowy } ;

składnik_jednostkowy                    = jednostka_prosta
                                        | '(' wyrażenie_multiplikatywne_jednostkowe ')' ;

przypisanie                             = id, '=', wyrażenie ;

definicja_funkcji                       = 'func', id, '(', lista_parametrów, ')', [ '->', typ ], blok_instrukcji ;
                                        (* brak -> ... oznacza funkcję niezwracającą wartości *)

lista_parametrów                        = [ parametr, { ',', parametr } ] ;

parametr                                = id, oznaczenie_typu ;

blok_instrukcji                         = '{' kod '}' ;

wywołanie_funkcji                       = id, '(', lista_argumentów ,')' ;

lista_argumentów                        = [ argument, { ',', argument } ] ;

argument                                = wyrażenie
                                        | string ;

string                                  = ciąg_znaków_w_cudzysłowach, { ciąg_znaków_w_cudzysłowach } ;

ciąg_znaków_w_cudzysłowach              = '"', { znak - '"' | escapowany_cudzysłów | odwołanie_do_zmiennej }, '"' ;

odwołanie_do_zmiennej                   = '{', id, [ 'in', jednostka ], '}' ;

instrukcja_return                       = 'return', [ wyrażenie ] ;

instrukcja_sufiksowa                    = id, sufiks_op ;

pętla_while                             = 'while', wyrażenie_logiczne, blok_instrukcji ;

instrukcja_if                           = 'if', wyrażenie_logiczne, blok_instrukcji, { blok_else_if }, [ blok_else ] ;

blok_else_if                            = 'else if', wyrażenie_logiczne, blok_instrukcji ;

blok_else                               = 'else', blok_instrukcji ;
```

### Słowa zarezerwowane (keywords):
```
bool
break
continue
else
false
func
if
in
print    //funkcja wbudowana
return
true
while
```

## Opis techniczny realizacji

Interpreter zostanie zaimplementowany przy użyciu języka C++.

Użyte narzędzia i bilioteki:
* `CMake` - budowanie projektu i zarządzanie zależnościami
* `GTest` - testy jednostkowe
* `boost.program_options` - obsługa parametrów wywołania w cmdline
* inne biblioteki `Boost`, jeśli na etapie implementacji zostaną uznane za przydatne

Interpreter będzie przyjmował jako wejście strumień znaków (kod źródłowy). Możliwości podania wejścia interpretera:
* stdin procesu interpretera
* nazwa pliku, którego zawartością jest kod do wykonania, podana jako cmdline parameter przy startowaniu procesu interpretera (`./interpret <file>`)

Wyjściem procesu interpretera będzie:
* exit status - liczba całkowita podana w instrukcji `return` w scopie globalnym podanego kodu źródłowego (poza ciałami funkcji), lub 0 jeśli taka instrukcja `return` jest nieobecna w wejściu
* stdout - ciągi znakowe będące wynikiem wołania funkcji `print` w podanym kodzie źródłowym

### Kwestie bezpieczeństwa:

* Limit długości identyfikatorów: 250 znaków
* Limit długości stałych tekstowych: 250 znaków
* Limit długości liczb: 250 znaków