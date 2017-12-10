# tmplengine
Very sloooow template engine on C++

# Описание
Основная идея этого шаблонизатора заключается в рекурсивном заполнении placeholder-ов вложенных друг в друга за один проход.
Чтобы избежать бесконечных рекурсивных вызовов вложенные placeholder-ы с таким же именем как у родительских вырезаются.

## Простой пример:

```
map<string, CustomType> values = {
  {"page",         CustomType("{{head}}{{body}}")},
  {"head",         CustomType("head")},
  {"body",         CustomType("{{header}} text {{footer}}")},
  {"header",       CustomType("header {{header}}")}, // <- вложенный {{header}} будет вырезан
  {"footer",       CustomType("footer")}
};

string input = "{{ page }}";

t->parse(input);
```

После заполнения placeholder-а парсер продолжает разбор строки с позиции, где начинался placeholder, т.е
```
text {{pls}}
text new text
     ^
```
При добавлении тильды ~ в начало имени placeholder-а {{ ~ pls}}, парсер продолжит разбирать строку не переходя в начальную позицию.
Соответственно вложенные placeholder-ы не будут обработаны.
```
text {{ ~ pls}}
text new text
            ^
```


## Поддерживается разбор простых условных выражений:

```
3 - 2 - -2 * - - (-(3 * 2)) + 0 <= -11

3-2--2*--(-(3*2))+0==-11   // даже если все всмятку

1 || 1 && 2 > 3 - 2

true == 1

!!!!!!false != true

-(true) == -1

1 && (-(true) == -1)
```

## Пример шаблона условного оператора

```
{%if -22+11 == -11 %}
  true
  
  {%if 1 == 0 %}
    false
  {%endif%}
  
{%endif%}
```

Следующие конструкции условного оператора не работают:
```
{% if 1 %}

{%else%}

{%endif%}

----------------

{% if 1 %}

{%else if 2 %}

{%endif%}

----------------

{% if 1 %}

{%else if 2 %}

{%else%}

{%endif%}
```

## Поддерживается вывод массива с данными:

Пример вызова
```
vector< map<string, string> > vc = {
  map<string, string>({
    {"name","Alex"},
    {"age","15"}
  })
  ,map<string, string>({
    {"name","Bob"},
    {"age","22"}
  })
};

string loop =
  "{% for item in items %}"
    "{{ ~ item.name }}"
    "{%if item.age <= 18 %} {{ ~ item.name }} {%endif%}"
  "{% endfor %}"
;

map<string, CustomType> values = {
  {"loop", CustomType(loop)}
};

string input = "{{ loop }}";

t->parse(input);
```

Пример шаблона
```
{% for item in items %}

  {{ ~ item.name }}

  {%if item.age <= 18 %} {{ ~ item.name }} {%endif%}
  
  {% for item in items %}

    {{ ~ item.name }}

    {%if item.age <= 18 %} {{ ~ item.name }} {%endif%}

  {% endfor %}

{% endfor %}
```

