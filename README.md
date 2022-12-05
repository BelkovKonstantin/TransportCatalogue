<h1 align="center">Транспортный справочник:bus:</h1>

## **Описание**
В данном репозитории реализована система хранения транспортных маршрутов и обработки запросов к ней.

### *Справочник может выводить:white_check_mark::*
* Информацию об автобусном маршруте X;
* Информацию об остановке X;
* SVG-представление карты всех маршрутов и остановок;
* Информацию о кратчайшем пути между остановками;

--------
<h2 align="center">Форматы ввода данных:pencil2:</h1>
Данные могут быть поданы на вход в двух вариантах: текстовом, JSON.

### *Текстовый*:one:
В первой строке потока ввода ожидается число N - количество запросов на обновление базы данных, затем - по одному на строке - вводятся сами запросы.
Запросы на обновление базы бывают двух типов.
* Запрос на добавление остановки: **```Stop S: latitude, longitude, D1m to stop1, D2m to stop2...```**;
>* **S** - название остановки;
>* **latitude** - широта, число типа double;
>* **longitude** - долгота, число типа double;
>* **D(i)m** - расстояние от остановки **S** до остановки **stop(i)** в метрах, целое число.
* Запросы на добавление маршрута:
    1. **```Bus B: stop1 - stop2 - ... stopN```**, автобус следует от stop1 до stopN и обратно с указанными промежуточными остановками;
    2. **```Bus B: stop1 > stop2 > ... > stopN > stop1```**, кольцевой маршрут с конечной stop1.
>* **B** - название маршрута;
>* **stop1, stop2...** - названия остановок, по которому следует маршрут.

После завершения ввода запросов на обновление базы, система ожидает ввода запросов на предоставление информации к справочнику.
Сначала также ожидается ввод числа M - количество запросов к справочнику, затем - по одному на строке - вводятся сами запросы.
Запросы на вывод информации также бывают двух типов.
1. Вывод информации о маршруте(автобусе): **```Bus B```**, где **B** название искомого автобуса.
2. Вывод информации об остановке: **```Stop S```**, где **S** название искомой остановки.

```
Внимание! Все запросы обязательны.
Порядок подачи запросов в текстовом формате важен: сначала запросы на обновление базы, потом запросы на вывод информации.
```

### *JSON*:two:
Данные подаются в поток ввода в следующем виде:
* **```{"base_requests":[...],"stat_requests":[...], "render_settings": { ... }, "routing_settings": {...} }```**

Обязательные массивы:
* **base_requests**
* **stat_requests**

Опциональные словари:star::
* **render_settings**
* **routing_settings**

```
Внимание! Порядок подачи запросов в JSON формате не важен.
Разделительные пробелы, табуляции и символы перевода строки внутри JSON могут располагаться
произвольным образом или вообще отсутствовать.
```

:arrow_right:Массив **```"base_requests":[...]```** содержит элементы двух типов: маршруты и остановки. Они перечисляются в произвольном порядке.
1. **```{ "type": "Bus", "name": "B", "stops": [ "stop1", "stop2", "stop3", "stopN" ], "is_roundtrip": true|false }```**
>* **type** - тип запроса, строка **"Bus"** означает, что словарь описывает автобусный маршрут;
>* **name** - название маршрута, в данном случае **B**;
>* **stops** - массив с названиями остановок, через которые проходит маршрут. У кольцевого маршрута название последней остановки дублирует название первой. Например: **["stop1", "stop2", "stop3", "stop1"]**;
>* **is_roundtrip** — значение типа bool. true, если маршрут кольцевой.
2. **```{ "type": "Stop", "name": "S", "latitude": 43.598701, "longitude": 39.730623, "road_distances": { "stop1": 1000, "stop2": 2000 }}```**
>* **type** - тип запроса, строка **"Stop"** означает, что словарь описывает остановку;
>* **name** - название остановки, в данном случае **S**;
>* **latitude** и **longitude** - широта и долгота остановки, числа типа double;
>* **road_distances** - массив, задающий дорожное расстояние от остановки **S** до соседних. Каждый ключ в этом словаре — название соседней остановки, значение — целочисленное расстояние в метрах.

:arrow_right:Массив **```"stat_requests":[...]```** может содержать элементы для вывода четырёх типов данных: маршруты и остановки. Они перечисляются в произвольном порядке.
1. **```{ "id": 1, "type": "Stop", "name": "S" }```**
>* **id** — уникальный числовой идентификатор запроса, целое число;
>* **type** - тип запроса, строка **"Stop"** означает, что требуется вывод информации об остановке;
>* **name** - название остановки, в данном случае **S**;
2. **```{ "id": 2, "type": "Bus", "name": "B" }```**
>* **type** - тип запроса, строка **"Bus"** означает, что требуется вывод информации об автобусном маршруте;
>* **name** - название маршрута, в данном случае **B**;
3. **```{ "id": 3, "type": "Map" }```**
>* **type** - тип запроса, строка **"Map"** означает, что требуется вывести SVG-представление карты маршрутов;
4. **```{ "id": 4, "type": "Route", "from": "stop1", "to": "stop2" }```**
>* **type** - тип запроса, строка **"Route"** означает, что требуется построить маршрут между двумя остановками;
>* **from** - точка из которой строится маршрут, в данном случае **stop1**;
>* **to** - конечная точка маршрута, в данном случае **stop2**;

:star:Для вывода SVG-представления карты маршрутов в JSON требуется указать и заполнить словарь **```"render_settings": { ... }```**.
Структура словаря **render_settings**:
```
"render_settings":
{
  "width": 1900.0,
  "height": 1080.0,
  "padding": 50.0,
  "line_width": 14.0,
  "stop_radius": 5.0,
  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 15.0],
  "stop_label_font_size": 20,
  "stop_label_offset": [7.0, -3.0],
  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,
  "color_palette": [
  "green",
  [255, 160, 0],
  "red"
  ]
}
```
>* **width** и **height** - ширина и высота изображения в пикселях. Вещественное число(double) в диапазоне от 0 до 100000.
>* **padding** - отступ краёв карты от границ SVG-документа. Вещественное число не меньше 0 и меньше min(width, height)/2.
>* **line_width** - толщина линий, которыми рисуются автобусные маршруты. Вещественное число в диапазоне от 0 до 100000.
>* **stop_radius** - радиус окружностей, которыми обозначаются остановки. Вещественное число в диапазоне от 0 до 100000.
>* **bus_label_font_size** - размер текста, которым написаны названия автобусных маршрутов. Целое число в диапазоне от 0 до 100000.
>* **bus_label_offset** - смещение надписи с названием маршрута относительно координат конечной остановки на карте. Массив из двух элементов типа double. Задаёт значения свойств dx и dy SVG-элемента <text>. Элементы массива — числа в диапазоне от –100000 до 100000.
>* **stop_label_font_size** - размер текста, которым отображаются названия остановок. Целое число в диапазоне от 0 до 100000.
>* **stop_label_offset** - смещение названия остановки относительно её координат на карте. Массив из двух элементов типа double. Задаёт значения свойств dx и dy SVG-элемента <text>. Числа в диапазоне от –100000 до 100000.
>* **underlayer_color** - цвет подложки под названиями остановок и маршрутов. Формат хранения цвета будет ниже.
>* **underlayer_width** - толщина подложки под названиями остановок и маршрутов. Задаёт значение атрибута stroke-width элемента <text>. Вещественное число в диапазоне от 0 до 100000.
>* **color_palette** - цветовая палитра. Непустой массив. Цвет можно указать в одном из следующих форматов:
>    * в виде строки, например, **"red"** или **"black"**;
>    * в массиве из трёх целых чисел диапазона [0, 255]. Они задают составляющие red, green, blue, например **[255, 15, 11]**;
>    * в массиве из четырёх элементов: три целых числа в диапазоне от [0, 255] и одно вещественное число в диапазоне от [0.0, 1.0]. Они задают составляющие red, green, blue и opacity(непрозрачность). Например **[255, 200, 23, 0.85]**.


:star:Для вывода маршрута между двумя остановками требуется указать и заполнить словарь **```"routing_settings": {...}```**
Структура словаря :
```
"routing_settings":
{
  "bus_wait_time": 3,
  "bus_velocity": 60
}
```
>* **bus_wait_time** - время ожидания автобуса на остановке, в минутах. Значение - целое число от 1 до 1000.
>* **bus_velocity** - скорость автобуса, в км/ч. Значение — вещественное число от 1 до 1000.
------
<h2 align="center">Форматы вывода данных:scroll:</h1>
Данные выводятся в двух вариантах, в зависимости от варианта ввода: текстовом, JSON.

### *Текстовый*:one:

* На запрос о маршруте **```Bus B```**, будет выведено **```Bus B: S stops on route, U unique stops, L route length, C curvature```**.
>* **B** - название маршрута. Оно совпадает с названием, переданным в запрос Bus;
>* **S** - количество остановок в маршруте автобуса от **stop1** до **stopN** включительно;
>* **U** - количество уникальных остановок, на которых останавливается автобус. Одинаковыми считаются остановки, имеющие одинаковые названия.
>* **L** - длина маршрута в метрах;
>* **С** - извилистость, то есть отношение фактической длины маршрута к географическому расстоянию. Фактическая длина маршрута - это **L**. Географическое расстояние - это расстояние между точками остановок на карте по прямой. Таким образом, **C** - вещественное число, большее единицы. Оно может быть равно единице только в том случае, если автобус едет между остановками по кратчайшему пути (и тогда больше похож на вертолёт). **С** может оказаться меньше единицы только благодаря телепортации или хордовым тоннелям.
>* Если маршрут **B** не найден, выводится **```Bus B: not found```**.

* На запрос об остановке **```Stop S```**, будет выведено **```Stop X: buses bus1 bus2 ... busN```**.
>* **bus1 bus2 ... busN** - список автобусов, проходящих через остановку. Дубли исключаются, названия отсортированы в алфавитном порядке;
>* Если остановка **S** не найдена, выводится **```Stop S: not found```**;
>* Если остановка **S** существует в базе, но через неё не проходят автобусы, выводится **```Stop S: no buses```**.

### *JSON*:two:

* На запрос об остановке **```{ "id": 1, "type": "Stop", "name": "S" }```**, вывод возможен следующий:
```
{
  "buses": [
    "B(1)", "B(2)", ... , "B(n)"
  ],
  "request_id": 1
}
```
>:arrow_up:Где **B(1)-B(n)** наиманование автобусов, в маршруте которых есть остановка **S**. **request_id** - уникальный идентификатор запроса.

```
{
  "buses": [
  ],
  "request_id": 1
}
```
>:arrow_up:Если остановка не участвует в маршрутах.

```
{
  "request_id": 1,
  "error_message": "not found"
}
```
>:arrow_up:Если остановка не найдена.

* На запрос о маршруте **```{ "id": 2, "type": "Bus", "name": "B" }```**, возможен такой вывод:
```
{
  "curvature": 1.23399,
  "request_id": 2,
  "route_length": 1700,
  "stop_count": 3,
  "unique_stop_count": 2
}
```
>* **curvature** — извилистость маршрута. Она равна отношению длины дорожного расстояния маршрута к длине географического расстояния. Число типа double;
>* **request_id** — должен быть равен id соответствующего запроса Bus. Целое число;
>* **route_length** — длина дорожного расстояния маршрута в метрах, целое число;
>* **stop_count** — количество остановок на маршруте;
>* **unique_stop_count** — количество уникальных остановок на маршруте.

* На запрос карты **```{ "id": 3, "type": "Map" }```**, возможен такой вывод:
```
{
  "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  
  <polyline points=\"80.17,150 20,20 80.17,150\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  
  <text fill=\"rgba(255,255,255,0.5)\" stroke=\"rgba(255,255,255,0.5)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"80.17\" y=\"150\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  
  <text fill=\"green\" x=\"80.17\" y=\"150\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  
  <text fill=\"rgba(255,255,255,0.5)\" stroke=\"rgba(255,255,255,0.5)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"20\" y=\"20\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  
  <text fill=\"green\" x=\"20\" y=\"20\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  
  <circle cx=\"100.817\" cy=\"170\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"30\" cy=\"30\" r=\"5\" fill=\"white\"/>\n  
  <text fill=\"rgba(255,255,255,0.5)\" stroke=\"rgba(255,255,255,0.5)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"100.817\" y=\"170\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop1</text>\n  
  <text fill=\"black\" x=\"80.17\" y=\"150\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop1</text>\n  
  <text fill=\"rgba(255,255,255,0.5)\" stroke=\"rgba(255,255,255,0.5)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"30\" y=\"30\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop3</text>\n  
  <text fill=\"black\" x=\"20\" y=\"20\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop3</text>\n</svg>",
  "request_id": 3
} 
```
>* В данном словаре ключу **map** соответствует значение равное SVG-представлению карты в формате JSON;
>* Если требуется текстовое SVG-представление карты, используйте функцию **GetMap**(метод **RequestHandler**, после заполения справочника в формате JSON).

* На запрос карты **```{ "id": 4, "type": "Route", "from": "stop1", "to": "stop2" }```**, возможен такой вывод:
```
{
    "items": [
        {
            "stop_name": "stop1",
            "time": 2,
            "type": "Wait"
        },
        {
            "bus": "B(1)",
            "span_count": 1,
            "time": 0.7,
            "type": "Bus"
        },
        {
            "stop_name": "stop3",
            "time": 2,
            "type": "Wait"
        },
        {
            "bus": "B(2)",
            "span_count": 1,
            "time": 5.8,
            "type": "Bus"
        }
    ],
    "request_id": 4,
    "total_time": 10.5
}
```
>* **items** — список элементов маршрута, бывают 2-х типов:
>    * **Wait** - ожидать **B(i)** автобуса **time** минут(**time** задается в **routing_settings**, по ключу **bus_wait_time**);
>    * **Bus** - проехать **span_count** остановок на автобусе **B(i)**, потратив **time** минут(**time** вычисляется из отношения расстояния, которое проедет автобус между данными остановками, на скорость автобуса, которая задаётся в **routing_settings**, по ключу **bus_velocity**);
```
{
    "request_id": 4,
    "error_message": "not found"
} 
```
>:arrow_up:Если маршрута между указанными остановками нет.
------
<h2 align="center">Примеры использования:eyes:</h1>

### *Текстовый ввод :arrow_right: текстовый вывод*
* Ввод:arrow_down::
```
22
Stop stop1: 55.909312, 37.586948, 15000m to stop2
Stop stop2: 55.909312, 37.443998, 30000m to stop3
Stop stop3: 55.733811, 37.443998, 15000m to stop4
Stop stop4: 55.733811, 37.586948
Bus B1: stop1 - stop2 - stop3 - stop4
Stop stop5: 55.820705, 37.678083, 9900m to stop6
Stop stop6: 55.820705, 37.738083
Bus B2: stop5 - stop6
Stop stop7: 55.850705, 37.708083, 9900m to stop8
Stop stop8: 55.790705, 37.708083
Bus B3: stop7 - stop8
Stop stop9: 55.820705, 37.828083, 9900m to stop10
Stop stop10: 55.820705, 37.888083
Bus B4: stop9 - stop10
Stop stop11: 55.850705, 37.858083, 9900m to stop12
Stop stop12: 55.790705, 37.858083
Bus B5: stop11 - stop12
Stop stop13: 55.939312, 37.426948, 50000m to stop14
Stop stop14: 55.939312, 37.906948, 35000m to stop15
Stop stop15: 55.703811, 37.906948, 50000m to stop16
Stop stop16: 55.703811, 37.426948, 35000m to stop13
Bus B6: stop13 > stop14 > stop15 > stop16 > stop13
3
Bus B1
Bus B6
Stop stop1
```
* Вывод:arrow_down::
```
Bus B1: 7 stops on route, 4 unique stops, 120000 route length, 1.6054 curvature
Bus B6: 5 stops on route, 4 unique stops, 170000 route length, 1.51326 curvature
Stop stop1: buses B1
```
### *JSON ввод :arrow_right: JSON вывод*
* Ввод:arrow_down::
```
{
"base_requests":[
{ "type": "Stop", "name": "stop1", "latitude": 55.909312, "longitude": 37.586948, 
"road_distances": { "stop2": 15000 }},
{ "type": "Stop", "name": "stop2", "latitude": 55.909312, "longitude": 37.443998, 
"road_distances": { "stop3": 30000 }},
{ "type": "Stop", "name": "stop3", "latitude": 55.733811, "longitude": 37.443998, 
"road_distances": { "stop4": 15000 }},
{ "type": "Stop", "name": "stop4", "latitude": 55.733811, "longitude": 37.586948, 
"road_distances": { }},
{ "type": "Stop", "name": "stop5", "latitude": 55.820705, "longitude": 37.678083, 
"road_distances": { "stop6": 9900 }},
{ "type": "Stop", "name": "stop6", "latitude": 55.820705, "longitude": 37.738083, 
"road_distances": {}},
{ "type": "Stop", "name": "stop7", "latitude": 55.850705, "longitude": 37.708083, 
"road_distances": { "stop8": 9900 }},
{ "type": "Stop", "name": "stop8", "latitude": 55.790705, "longitude": 37.708083, 
"road_distances": {}},
{ "type": "Stop", "name": "stop9", "latitude": 55.820705, "longitude": 37.828083, 
"road_distances": { "stop10": 9900 }},
{ "type": "Stop", "name": "stop10", "latitude": 55.820705, "longitude": 37.888083, 
"road_distances": {}},
{ "type": "Stop", "name": "stop11", "latitude": 55.850705, "longitude": 37.858083, 
"road_distances": { "stop12": 9900 }},
{ "type": "Stop", "name": "stop12", "latitude": 55.790705, "longitude": 37.858083, 
"road_distances": {}},
{ "type": "Stop", "name": "stop13", "latitude": 55.939312, "longitude": 37.426948, 
"road_distances": { "stop14": 50000 }},
{ "type": "Stop", "name": "stop14", "latitude": 55.939312, "longitude": 37.906948, 
"road_distances": { "stop15": 35000 }},
{ "type": "Stop", "name": "stop15", "latitude": 55.703811, "longitude": 37.906948, 
"road_distances": { "stop16": 50000 }},
{ "type": "Stop", "name": "stop16", "latitude": 55.703811, "longitude": 37.426948, 
"road_distances": { "stop13": 35000 }},
{ "type": "Bus", "name": "B1", 
"stops": [ "stop1", "stop2", "stop3", "stop4" ], "is_roundtrip": false },
{ "type": "Bus", "name": "B2", 
"stops": [ "stop5", "stop6" ], "is_roundtrip": false },
{ "type": "Bus", "name": "B3", 
"stops": [ "stop7", "stop8"  ], "is_roundtrip": false },
{ "type": "Bus", "name": "B4", 
"stops": [ "stop9", "stop10"  ], "is_roundtrip": false },
{ "type": "Bus", "name": "B5", 
"stops": [ "stop11", "stop12"  ], "is_roundtrip": false },
{ "type": "Bus", "name": "B6", 
"stops": [ "stop13", "stop14", "stop15", "stop16", "stop13" ], "is_roundtrip": true }
	],
"stat_requests":[
{ "id": 1, "type": "Bus", "name": "B1" },
{ "id": 2, "type": "Bus", "name": "B6" },
{ "id": 3, "type": "Stop", "name": "stop1"},
{ "id": 4, "type": "Map" },
{ "id": 5, "type": "Route", "from": "stop1", "to": "stop3" },
{ "id": 6, "type": "Route", "from": "stop3", "to": "stop15" }
	], 
"render_settings": {
  "width": 1700.0,
  "height": 880.0,
  "padding": 100.0,
  "line_width": 14.0,
  "stop_radius": 5.0,
  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 15.0],
  "stop_label_font_size": 20,
  "stop_label_offset": [7.0, -3.0],
  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,
  "color_palette": [
  "green",
  [255, 160, 0],
  "red",
[255, 160, 0],
  "red",
"yellow"
  ]
}, 
"routing_settings": {
  "bus_wait_time": 2,
  "bus_velocity": 60
	}
}
```
* Вывод:arrow_down::
```
[
    {
        "curvature": 1.6054,
        "request_id": 1,
        "route_length": 120000,
        "stop_count": 7,
        "unique_stop_count": 4
    },
    {
        "curvature": 1.51326,
        "request_id": 2,
        "route_length": 170000,
        "stop_count": 5,
        "unique_stop_count": 4
    },
    {
        "buses": [
            "B1"
        ],
        "request_id": 3
    },
    {
        "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  
<polyline points=\"561.994,186.624 149.231,186.624 149.231,693.376 561.994,693.376 149.231,693.376 149.231,186.624 561.994,186.624\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  
<polyline points=\"825.143,442.473 998.39,442.473 825.143,442.473\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  
<polyline points=\"911.766,355.849 911.766,529.097 911.766,355.849\" fill=\"none\" stroke=\"red\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  
<polyline points=\"1258.26,442.473 1431.51,442.473 1258.26,442.473\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  
<polyline points=\"1344.89,355.849 1344.89,529.097 1344.89,355.849\" fill=\"none\" stroke=\"red\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  
<polyline points=\"100,100 1485.98,100 1485.98,780 100,780 100,100\" fill=\"none\" stroke=\"yellow\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"561.994\" y=\"186.624\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B1</text>\n  
<text fill=\"green\" x=\"561.994\" y=\"186.624\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B1</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"561.994\" y=\"693.376\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B1</text>\n  
<text fill=\"green\" x=\"561.994\" y=\"693.376\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B1</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"825.143\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B2</text>\n  
<text fill=\"rgb(255,160,0)\" x=\"825.143\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B2</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"998.39\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B2</text>\n  
<text fill=\"rgb(255,160,0)\" x=\"998.39\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B2</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"911.766\" y=\"355.849\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B3</text>\n  
<text fill=\"red\" x=\"911.766\" y=\"355.849\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B3</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"911.766\" y=\"529.097\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B3</text>\n  
<text fill=\"red\" x=\"911.766\" y=\"529.097\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B3</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1258.26\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B4</text>\n  
<text fill=\"rgb(255,160,0)\" x=\"1258.26\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B4</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1431.51\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B4</text>\n  
<text fill=\"rgb(255,160,0)\" x=\"1431.51\" y=\"442.473\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B4</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1344.89\" y=\"355.849\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B5</text>\n  
<text fill=\"red\" x=\"1344.89\" y=\"355.849\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B5</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1344.89\" y=\"529.097\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B5</text>\n  
<text fill=\"red\" x=\"1344.89\" y=\"529.097\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B5</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"100\" y=\"100\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B6</text>\n  
<text fill=\"yellow\" x=\"100\" y=\"100\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">B6</text>\n  
<circle cx=\"561.994\" cy=\"186.624\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"1431.51\" cy=\"442.473\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"1344.89\" cy=\"355.849\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"1344.89\" cy=\"529.097\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"100\" cy=\"100\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"1485.98\" cy=\"100\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"1485.98\" cy=\"780\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"100\" cy=\"780\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"149.231\" cy=\"186.624\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"149.231\" cy=\"693.376\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"561.994\" cy=\"693.376\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"825.143\" cy=\"442.473\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"998.39\" cy=\"442.473\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"911.766\" cy=\"355.849\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"911.766\" cy=\"529.097\" r=\"5\" fill=\"white\"/>\n  
<circle cx=\"1258.26\" cy=\"442.473\" r=\"5\" fill=\"white\"/>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"561.994\" y=\"186.624\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop1</text>\n  
<text fill=\"black\" x=\"561.994\" y=\"186.624\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop1</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1431.51\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop10</text>\n  
<text fill=\"black\" x=\"1431.51\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop10</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1344.89\" y=\"355.849\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop11</text>\n  
<text fill=\"black\" x=\"1344.89\" y=\"355.849\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop11</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1344.89\" y=\"529.097\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop12</text>\n  
<text fill=\"black\" x=\"1344.89\" y=\"529.097\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop12</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"100\" y=\"100\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop13</text>\n  
<text fill=\"black\" x=\"100\" y=\"100\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop13</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1485.98\" y=\"100\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop14</text>\n  
<text fill=\"black\" x=\"1485.98\" y=\"100\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop14</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1485.98\" y=\"780\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop15</text>\n  
<text fill=\"black\" x=\"1485.98\" y=\"780\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop15</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"100\" y=\"780\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop16</text>\n  
<text fill=\"black\" x=\"100\" y=\"780\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop16</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"149.231\" y=\"186.624\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop2</text>\n  
<text fill=\"black\" x=\"149.231\" y=\"186.624\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop2</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"149.231\" y=\"693.376\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop3</text>\n  
<text fill=\"black\" x=\"149.231\" y=\"693.376\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop3</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"561.994\" y=\"693.376\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop4</text>\n  
<text fill=\"black\" x=\"561.994\" y=\"693.376\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop4</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"825.143\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop5</text>\n  
<text fill=\"black\" x=\"825.143\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop5</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"998.39\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop6</text>\n  
<text fill=\"black\" x=\"998.39\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop6</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"911.766\" y=\"355.849\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop7</text>\n  
<text fill=\"black\" x=\"911.766\" y=\"355.849\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop7</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"911.766\" y=\"529.097\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop8</text>\n  
<text fill=\"black\" x=\"911.766\" y=\"529.097\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop8</text>\n  
<text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"1258.26\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop9</text>\n  
<text fill=\"black\" x=\"1258.26\" y=\"442.473\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">stop9</text>\n</svg>",
        "request_id": 4
    },
    {
        "items": [
            {
                "stop_name": "stop1",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "B1",
                "span_count": 2,
                "time": 45,
                "type": "Bus"
            }
        ],
        "request_id": 5,
        "total_time": 47
    },
    {
        "error_message": "not found",
        "request_id": 6
    }
]
```
### *Вывод текстового SVG-представления карты*:sparkles:
При использовании функции **GetMap** можно получить текстовое SVG-представление этой же карты:
```
<?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
  <polyline points="561.994,186.624 149.231,186.624 149.231,693.376 561.994,693.376 149.231,693.376 149.231,186.624 561.994,186.624" fill="none" stroke="green" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="825.143,442.473 998.39,442.473 825.143,442.473" fill="none" stroke="rgb(255,160,0)" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="911.766,355.849 911.766,529.097 911.766,355.849" fill="none" stroke="red" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="1258.26,442.473 1431.51,442.473 1258.26,442.473" fill="none" stroke="rgb(255,160,0)" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="1344.89,355.849 1344.89,529.097 1344.89,355.849" fill="none" stroke="red" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="100,100 1485.98,100 1485.98,780 100,780 100,100" fill="none" stroke="yellow" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="561.994" y="186.624" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B1</text>
  <text fill="green" x="561.994" y="186.624" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B1</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="561.994" y="693.376" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B1</text>
  <text fill="green" x="561.994" y="693.376" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B1</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="825.143" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B2</text>
  <text fill="rgb(255,160,0)" x="825.143" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B2</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="998.39" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B2</text>
  <text fill="rgb(255,160,0)" x="998.39" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B2</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="911.766" y="355.849" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B3</text>
  <text fill="red" x="911.766" y="355.849" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B3</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="911.766" y="529.097" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B3</text>
  <text fill="red" x="911.766" y="529.097" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B3</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1258.26" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B4</text>
  <text fill="rgb(255,160,0)" x="1258.26" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B4</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1431.51" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B4</text>
  <text fill="rgb(255,160,0)" x="1431.51" y="442.473" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B4</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1344.89" y="355.849" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B5</text>
  <text fill="red" x="1344.89" y="355.849" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B5</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1344.89" y="529.097" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B5</text>
  <text fill="red" x="1344.89" y="529.097" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B5</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="100" y="100" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B6</text>
  <text fill="yellow" x="100" y="100" dx="7" dy="15" font-size="20" font-family="Verdana" font-weight="bold">B6</text>
  <circle cx="561.994" cy="186.624" r="5" fill="white"/>
  <circle cx="1431.51" cy="442.473" r="5" fill="white"/>
  <circle cx="1344.89" cy="355.849" r="5" fill="white"/>
  <circle cx="1344.89" cy="529.097" r="5" fill="white"/>
  <circle cx="100" cy="100" r="5" fill="white"/>
  <circle cx="1485.98" cy="100" r="5" fill="white"/>
  <circle cx="1485.98" cy="780" r="5" fill="white"/>
  <circle cx="100" cy="780" r="5" fill="white"/>
  <circle cx="149.231" cy="186.624" r="5" fill="white"/>
  <circle cx="149.231" cy="693.376" r="5" fill="white"/>
  <circle cx="561.994" cy="693.376" r="5" fill="white"/>
  <circle cx="825.143" cy="442.473" r="5" fill="white"/>
  <circle cx="998.39" cy="442.473" r="5" fill="white"/>
  <circle cx="911.766" cy="355.849" r="5" fill="white"/>
  <circle cx="911.766" cy="529.097" r="5" fill="white"/>
  <circle cx="1258.26" cy="442.473" r="5" fill="white"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="561.994" y="186.624" dx="7" dy="-3" font-size="20" font-family="Verdana">stop1</text>
  <text fill="black" x="561.994" y="186.624" dx="7" dy="-3" font-size="20" font-family="Verdana">stop1</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1431.51" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop10</text>
  <text fill="black" x="1431.51" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop10</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1344.89" y="355.849" dx="7" dy="-3" font-size="20" font-family="Verdana">stop11</text>
  <text fill="black" x="1344.89" y="355.849" dx="7" dy="-3" font-size="20" font-family="Verdana">stop11</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1344.89" y="529.097" dx="7" dy="-3" font-size="20" font-family="Verdana">stop12</text>
  <text fill="black" x="1344.89" y="529.097" dx="7" dy="-3" font-size="20" font-family="Verdana">stop12</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="100" y="100" dx="7" dy="-3" font-size="20" font-family="Verdana">stop13</text>  
  <text fill="black" x="100" y="100" dx="7" dy="-3" font-size="20" font-family="Verdana">stop13</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1485.98" y="100" dx="7" dy="-3" font-size="20" font-family="Verdana">stop14</text>
  <text fill="black" x="1485.98" y="100" dx="7" dy="-3" font-size="20" font-family="Verdana">stop14</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1485.98" y="780" dx="7" dy="-3" font-size="20" font-family="Verdana">stop15</text>
  <text fill="black" x="1485.98" y="780" dx="7" dy="-3" font-size="20" font-family="Verdana">stop15</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="100" y="780" dx="7" dy="-3" font-size="20" font-family="Verdana">stop16</text>  
  <text fill="black" x="100" y="780" dx="7" dy="-3" font-size="20" font-family="Verdana">stop16</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="149.231" y="186.624" dx="7" dy="-3" font-size="20" font-family="Verdana">stop2</text>
  <text fill="black" x="149.231" y="186.624" dx="7" dy="-3" font-size="20" font-family="Verdana">stop2</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="149.231" y="693.376" dx="7" dy="-3" font-size="20" font-family="Verdana">stop3</text>
  <text fill="black" x="149.231" y="693.376" dx="7" dy="-3" font-size="20" font-family="Verdana">stop3</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="561.994" y="693.376" dx="7" dy="-3" font-size="20" font-family="Verdana">stop4</text>
  <text fill="black" x="561.994" y="693.376" dx="7" dy="-3" font-size="20" font-family="Verdana">stop4</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="825.143" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop5</text>
  <text fill="black" x="825.143" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop5</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="998.39" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop6</text>
  <text fill="black" x="998.39" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop6</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="911.766" y="355.849" dx="7" dy="-3" font-size="20" font-family="Verdana">stop7</text>
  <text fill="black" x="911.766" y="355.849" dx="7" dy="-3" font-size="20" font-family="Verdana">stop7</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="911.766" y="529.097" dx="7" dy="-3" font-size="20" font-family="Verdana">stop8</text>
  <text fill="black" x="911.766" y="529.097" dx="7" dy="-3" font-size="20" font-family="Verdana">stop8</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="1258.26" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop9</text>
  <text fill="black" x="1258.26" y="442.473" dx="7" dy="-3" font-size="20" font-family="Verdana">stop9</text>
</svg>
```
>Которое при сохранении в отдельный документ **svg** формата, можно вывести с помощью браузера:arrow_down:
>![SVG-карта](https://svgshare.com/i/oaT.svg "Жмак сюда")
