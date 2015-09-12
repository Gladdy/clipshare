from clipshare.models import User

class EmailAuthBackend(object):
    """
    A custom authentication backend. Allows users to log in using their email address.
    """

    def authenticate(self, email=None, password=None):
        """
        Authentication method
        """
        user = User.objects.get(email=email)
        if user.check_password(password):
            return user
        else:
            raise User.DoesNotExist

    def get_user(self, user_id):
        user = User.objects.get(pk=user_id)
        if user.is_active:
            return user
        else:
            raise User.DoesNotExist
