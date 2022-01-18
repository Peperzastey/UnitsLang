# tkom-SIunits

Celem projektu jest implementacja interpretera języka programowania.

Przedstawiony język programowania ma umożliwiać wykonywanie operacji arytmetycznych na wartościach liczbowych z jednostkami z układu SI,
wraz ze sprawdzaniem poprawności operacji na poziomie zgodności jednostek operandów.
Umożliwia definiowanie własnych funkcji celem reużycia fragmentów kodu, obsługuje rekurencję i zmienne lokalne funkcji.

Typowanie w przedstawionym języku jest statyczne i silne.

## Przykładowy kod źródłowy
```
length = 2[cm]
width = 500 [mm]

area = 14[m2] + (length * width) - 10[N] / 6[Pa]
print( "Powierzchnia wynosi {area}, czyli {area in [cm2]}" )
// stdout: Powierzchnia wynosi 15.677m2, czyli 156 766.667cm2

// wynik domyślnie jest wyświelany z przedrostkiem wielokrotności
// obecnym w układzie SI dla danej jednostki - czyli poza kg wszystkie
// jednostki są domyślnie bez przedrostków
print( "Escapowanie: \{ 1Pa = 1N/1m2 \} \"ciśnienie\"" )

// istnieje możliwość podania liczby w grupach 3cyfrowych oddzielonych spacją
mass = 23 001 250.5[mg]
// \ na końcu linii umożliwia kontynuację instrukcji w następnej linii
calculated_work = (mass * 2 * area * 2.3[km]) \
    / (length * 5[min] * 2 000[us])
print( "To wyliczenie pewnie nie ma sensu, ale energia wyniosła" \
    "{calculated_work}" )

area = 500[cm2]
acc = 2 [m/s2]
dist = acc * 0.5[h] * 2[s]
speed = dist * 15[Hz]

pressure1 = 5[kg/(m*s2)]
pressure2 = 14 432 [kg/m/s2]

length = 2.5[cm]
width = 2[cm]
area = length * width
while area < 2[m2] {
    width = width * 1.5
    area = length * width
}

if area > 3[m2] {
    print( "Area is way too big" )
} elif area > 2.5[m2] {
    print( "Area is slightly too big" )
} else {
    print( "Area is acceptable" )
}

// parametry funkcji przekazywane są przez wartość
func fibonacci_meters (steps [1]) -> [m] {
    // indentacja bloku nie jest wymagana
    if steps == 0 {
        return 0[m]
    } elif steps == 1 {
        return 1[m]
    } else {
        // rekursja
        return fibonacci_meters(steps - 1) + fibonacci_meters(steps - 2)
    }
}

func fibonacci_speed_iterative (steps [1]) -> [m/s] {
    // zmienne lokalne
    elem0 = 0[m/s]
    elem1 = 1[m/s]
    count = 1
    while count < steps {
        elem1 = elem1 + elem0
        elem0 = elem1 - elem0
        count = count + 1
    }
    return elem1
}

meters_after_16_steps = fibonacci_meters(16)
// możliwość nadania z góry typu dla zmiennej, w tym wypadku jeśli
// wartość przypisywanego wyrażenia nie ma zgodnego typu - rzucony
// będzie błąd
distance[m] = fibonacci_speed_iterative(20) * 16[s]
// elem0, elem1 i temp z ciała funkcji fibonacci_speed_iterative nie są tu widoczne
```

Przykłady nieprawidłowych operacji (niezgodność jednostek):
```
15[m] + 3[s]
3 000[s2] - 15[s] * 2[ms] / 0.5[s]
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

mult_op                                 = '*' | '/' ;

add_op                                  = '+' | '-' ;

rel_op                                  = '<' | '>' | '<=' | '>=' ;

equal_op                                = '==' | '!=' ;

and_op                                  = '&&' ;

or_op                                   = '||' ;

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
                                        | definicja_funkcji
                                        } ;

tekst                                   = { znak } ;

instrukcja                              = [
                                          definicja_zmiennej
                                        | przypisanie
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

oznaczenie_jednostki                    = '[', jednostka, ']' ;

typ                                     = jednostka | skalar | 'bool' | 'str' ;

wyrażenie                               = wyrażenie_log_or
                                        | string ;
                                        
wyrażenie_log_or                        = wyrażenie_log_and, { or_op, wyrażenie_log_and }

wyrażenie_log_and                       = wyrażenie_log_eq, { and_op, wyrażenie_log_eq } ;

wyrażenie_log_eq                        = wyrażenie_log_rel, { equal_op, wyrażenie_log_rel } ;

wyrażenie_log_rel                       = wyrażenie_addytywne, { rel_op, wyrażenie_addytywne }
                                        | wartość_logiczna ;

wyrażenie_addytywne                     = wyrażenie_multiplikatywne, { add_op, wyrażenie_multiplikatywne } ;

wyrażenie_multiplikatywne               = składnik, { mult_op, składnik } ;

składnik                                = id
                                        | wartość
                                        | wywołanie_funkcji
                                        | '(' wyrażenie_log_or ')' ;

wartość                                 = liczba, [ oznaczenie_jednostki ] ;
                                        (* brak jednostki oznacza skalara *)

jednostka                               = składnik_jednostkowy, { mult_op, składnik_jednostkowy } ;

składnik_jednostkowy                    = jednostka_prosta
                                        | '(' jednostka ')' ;

jednostka_prosta                        = [ prefix ], jednostka_podstawowa, [ potega_jednostki ] ;

potega_jednostki                        = '2' | '3' ;

przypisanie                             = id, '=', wyrażenie ;

definicja_funkcji                       = 'func', id, '(', lista_parametrów, ')', [ '->', oznaczenie_typu ], blok_instrukcji ;
                                        (* brak -> ... oznacza funkcję niezwracającą wartości *)

lista_parametrów                        = [ parametr, { ',', parametr } ] ;

parametr                                = id, oznaczenie_typu ;

blok_instrukcji                         = '{' instrukcja '}' ;

wywołanie_funkcji                       = id, '(', lista_argumentów ,')' ;

lista_argumentów                        = [ argument, { ',', argument } ] ;

argument                                = wyrażenie ;

string                                  = ciąg_znaków_w_cudzysłowach ;

ciąg_znaków_w_cudzysłowach              = '"', { znak - '"' | escapowany_cudzysłów | odwołanie_do_zmiennej }, '"' ;

odwołanie_do_zmiennej                   = '{', id, [ 'in', oznaczenie_jednostki ], '}' ;

instrukcja_return                       = 'return', [ wyrażenie ] ;

instrukcja_sufiksowa                    = id, sufiks_op ;

pętla_while                             = 'while', wyrażenie, blok_instrukcji ;

instrukcja_if                           = 'if', wyrażenie, blok_instrukcji, { blok_else_if }, [ blok_else ] ;

blok_else_if                            = 'elif', wyrażenie, blok_instrukcji ;

blok_else                               = 'else', blok_instrukcji ;
```

### Słowa zarezerwowane (keywords):
```
bool
break
continue
elif
else
false
func
if
in
print    //funkcja wbudowana
return
str
true
while
```

## Opis techniczny realizacji

Interpreter zostanie zaimplementowany przy użyciu języka C++.

Użyte narzędzia i bilioteki:
* `CMake` - budowanie projektu i zarządzanie zależnościami
* `GTest` - testy jednostkowe

Interpreter będzie przyjmował jako wejście strumień znaków (kod źródłowy). Możliwości podania wejścia interpretera:
* stdin procesu interpretera
* nazwa pliku, którego zawartością jest kod do wykonania, podana jako cmdline parameter przy startowaniu procesu interpretera (`./interpret <file>`)

Wyjściem procesu interpretera będzie:
* exit status - liczba całkowita podana w instrukcji `return` w scopie globalnym podanego kodu źródłowego (poza ciałami funkcji), lub 0 jeśli taka instrukcja `return` jest nieobecna w wejściu
* stdout - ciągi znakowe będące wynikiem wołania funkcji `print` w podanym kodzie źródłowym

Dostępne typy danych:
* liczba z jednostką
* skalar (w kodzie interpretera traktowany jest jako specjalny rodzaj jednostki)
* bool - wartość logiczna `true`/`false`
* string

Nie ma rozróżnienia na typ całkowitoliczbowy i (zmienno)przecinkowy. Interpreter traktuje wartość zmiennych liczbowych jako typ `double`

Instrukcje blokowe tworzą osobny scope dla zmiennych. W nowym scopie możliwe jest odwołanie się i modyfikacja wartości zmiennych w scopach-rodzicach
oraz w global scopie.

Wywołanie fukncji powoduje powstanie nowego `FunctionCallContext` na stosie. W ciele funkcji istnieje możliwość odwołania się i modyfikacji jedynie
argumentów wywołania tej funkcji oraz zmiennych w global scopie.

Możliwe jest przykrywanie (shadowing) parametrów funkcji przez definicję zmiennej o tej samej nazwie (oraz innym lub tym samym typie)
wewnątrz ciała funkcji - wartości przekazane jako argumenty wywołania funkcji mają osobny scope od zmiennych zdefiniowanych w ciele funkcji

Argumenty wywołania funkcji przekazywane są jedynie przez wartość, tak samo wartość zwracana z funkcji.

Nie ma funkcji `main`. Zamiast tego interpreter zaczyna wykonywać napisany kod poczynając od pierwszej instrukcji niebędącej definicją funkcji
(podobnie jak w języku Python)

Zmienne definiowane poza blokami `{...}` (czyli blokami dla instruckji `if`/`elif`/`else`, `while` oraz blokiem-ciałem definicji funkcji)
należą do global scope

### Podział kodu na moduły i klasy

Moduły:
* **`source`**: odpowiedzialny za dostarczenie interfejsu oraz implementacji pobierania kolejnych znaków oraz aktualnej pozycji w strumieniu wejściowym dla `Lexera`
    * Klasy:
        * **`Source`**: interfejs
        * **`FileSource`**: implementacja dostarczająca kolejne znaki z zadanego pliku
        * **`StringSource`**: implementacja dostarczająca kolejne znaki z ciągu znakowego; umożliwia testy jednostkowe kolejnych modułów
* **`lexer`**: zależny od modułu `source` i `error`; odpowiedzialny za analizę leksykalną
    * Klasy:
        * **`Lexer`**: dostarcza metodę `getToken()` zwracającą kolejny `Token` języka skonstruowany ze znaków od `Source`, lub błąd jeśli się nie powiodło
        * **`Token`**: struktura opisująca token języka; zawiera typ, wartość oraz pozycję pierwszego znaku tokena w strumieniu wejściowym
        * **`TokenType`**: enum opisujący typy tokenów
        * **`Unit`**: jedna z możliwych wartości tokenu; opisuje jednostkę dla wartości liczbowej w języku; zawiera prefix, typ jednostki oraz potęgę
        * **`UnitType`**: enum opisujący typy jednostek
        * **`String`**: jedna z możliwych wartości tokenu; opisuje ciąg znakowy w języku; zawiera listę sub-tokenów w celu realizacji formatowania
        (np. `"a = {a}"`)
* **`parser`**: zależny od modułu `lexer`, `error` i `codeObjects`; odpowiedzialny za analizę składniową
    * Klasy:
        * **`Parser`**: dostarcza metodę `parse()` zwracającą obiekt `Program` z modułu `codeObjects` opisujący strukturę programu, lub błąd jeśli się nie powiodło
* **`codeObjects`**: zależny od modułu `error`; zawiera klasy reprezentujące konstrukcje języka oraz ich logikę; zawiera klasę `Interpreter`
    * Klasy:
        * **`Program`**: reprezentuje powstału program; dostarcza metodę `execute(interpreter)` umożliwiającą wykonanie programu w kontekście dostarczonego obiektu interpretera; zawiera słownik `FuncDefs` oraz `InstructionBlock` zawierający listę instrukcji do wykonania
        * **`FuncDef`**: reprezentuje definicję funkcji; dostarcza metodę `call(interpreter, args)` umożliwiającą wykonanie funkcji z dostarczoną listą argumentów; korzysta z `InstructionBlock` jako ciała funkcji; zawiera listę `Variables`(lista parametrów)
        * **`Variable`**: reprezentuje parę nazwa - typ(`Type`)
        * **`InstructionBlock`**: reprezentuje blok instrukcji; dostarcza metodę `execute(interpreter)`; zawiera listę `Instructions`
        * **`Instruction`**: abstrakcyjny interfejs dla instrukcji; dostarcza metodę `execute(interpreter)` zwracającą obiekt `InstrResult`
        * **`InstrResult`**: enum opisujący typy wyników wykonania instrukcji (NORMAL, RETURN, BREAK, CONTINUE)
        * **`Expression`**: abstrakcyjny interfejs dla wyrażenia; dostarcza metodę `calculate(interpreter)` zwracającą obiekt `Value` oraz `getRPN()` zwracającą string w celu testowania jednostkowego
        * **`Value`**: implementacja `Expression`; opisuje parę wartość(double/bool/string) - typ(`Type`)
        * **`Type`**: opisuje typ wartości w języku; zawiera `Type::TypeClass` oraz `Unit`
        * **`Type::TypeClass`**: enum opisujący typy danych w języku
        * **`Unit`**: opisuje typ jednostkowy oraz skalarny w języku; zawiera metody wyznaczające jednostkę wynikową operacji arytmetycznych
        * **`BinaryExpression`** : implementacja `Expression`; reprezentuje operację binarną; zawiera 2 `Expression` - lewy i prawy operand oraz operator
        * **`VarReference`**: implementacja `Expression`; reprezentuje odwołanie do wartości zmiennej
        * **`String`**: implementacja `Expression`; reprezentuje ciąg znakowy w języku - osobny typ od Value w celu realizacji formatowania; (w tym celu) zawiera listę `Values`
        * **`FuncCall`**: implementacja `Instruction` oraz `Expression`; zawiera listę `Expression`(argumenty)
        * **`If`**: implementacja `Instruction`; reprezentuje instrukcję if/elif/else w języku; zawiera `Expression`(warunek), `InstructionBlock`(blok true) oraz wskazanie na `If`(przynależny elif/else - implementacja łańcuchowa)
        * **`While`**: implementacja `Instruction`; zawiera `Expression`(warunek), `InstructionBlock`
        * **`Continue`**: implementacja `Instruction`
        * **`Break`**: implementacja `Instruction`
        * **`Return`**: implementacja `Instruction`; zawiera opcjonalny `Expression`(wartość zwracana)
        * **`VarDefOrAssignment`**: implementacja `Instruction`; reprezentuje instrukcję definicji zmiennej lub przypisania do zmiennej w języku; zawiera `Expression`(wartość dla zmiennej)
        * **`InternalPrintInstr`**: implementacja `Instruction`; realizuje wypisanie ciągu znakowego do stdout Interpretera w ciele wbudowanej funkcji print()
        * **`Interpreter`**: dostarcza metodę `executeProgram()` wykonującą obiekt `Program`; dostarcza obiektom instrukcji metody do operacji na zmiennych i funkcjach, realizuje te operacje; realizuje stos wywołań, scopy dla zmiennych, zwracanie wartości z funkcji, pisanie do stdout
        * **`FuncCallContext`**: reprezentuje kontekst dla wywołania funkcji; dostarcza metod do tworzenia i usuwania subscopów, dostępu i tworzenia zmiennych; zawiera listę Scope-ów (słowników nazwa - wartość(`Value`))
* **`error`**: odpowiedzialny za obsługę błędów zgłaszanych przez pozostałe moduły
    * Klasy:
        * **`ErrorHandler`**: dostarcza metod zgłaszania błędów z wyróżnieniem modułu, z którego pochodzi zgłoszenie

### Kwestie bezpieczeństwa:

* Limit długości identyfikatorów: 250 znaków
* Limit długości stałych tekstowych: 250 znaków
* Limit długości liczb przed przecinkiem: 25 znaków
* Limit długości liczb po przecinku: 12 znaków
