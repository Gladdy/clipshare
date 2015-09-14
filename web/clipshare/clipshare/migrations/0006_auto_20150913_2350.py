# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('clipshare', '0005_auto_20150913_2251'),
    ]

    operations = [
        migrations.AlterField(
            model_name='file',
            name='location',
            field=models.FilePathField(),
        ),
        migrations.AlterField(
            model_name='file',
            name='url',
            field=models.URLField(),
        ),
    ]
