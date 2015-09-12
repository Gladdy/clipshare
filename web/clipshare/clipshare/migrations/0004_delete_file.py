# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('clipshare', '0003_file'),
    ]

    operations = [
        migrations.DeleteModel(
            name='File',
        ),
    ]
